/***
  This file is part of PaSystray

  Copyright (C) 2011-2013  Christoph Gysin

  PaSystray is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  PaSystray is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with PaSystray; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include "ui.h"
#include "config.h"
#include "systray_impl.h"

static GtkBuilder* builder;

void ui_load()
{
    builder = gtk_builder_new();

    GError* error = NULL;

    // GLADE_FILE is set in src/Makefile.am
    const char* filename = GLADE_FILE;

    /* try to load ui in current dir first */
    char* local_file = "pasystray.glade";
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;
    local_file = "src/pasystray.glade";
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;

    g_debug("using UI file: %s", filename);

    guint ret = gtk_builder_add_from_file(builder, filename, &error);

    if(!ret)
    {
        g_error("[ui] %s", error->message);
    }

    gtk_about_dialog_set_version(ui_aboutdialog(), PACKAGE_VERSION);
    gtk_about_dialog_set_website(ui_aboutdialog(), PACKAGE_URL);
    gtk_about_dialog_set_website_label(ui_aboutdialog(), PACKAGE_URL);
}

void ui_update_systray_icon(menu_info_item_t* mii)
{
    g_debug("pulseaudio_update_systray_icon(%s)", mii->name);

    pa_volume_t volume = pa_cvolume_avg(mii->volume);

    g_debug("volume:%u%s", volume, mii->mute ? " muted" : "");

    const char* icon_name = NULL;

    if(volume == PA_VOLUME_MUTED || mii->mute)
        icon_name = "audio-volume-muted";
    else if(volume < (PA_VOLUME_NORM / 3))
        icon_name = "audio-volume-low";
    else if(volume < (PA_VOLUME_NORM / 3 * 2))
        icon_name = "audio-volume-medium";
    else
        icon_name = "audio-volume-high";

    menu_infos_t* mis = mii->menu_info->menu_infos;
    systray_impl_set_icon(mis->systray, icon_name);
}

GtkAboutDialog* ui_aboutdialog()
{
    return (GtkAboutDialog*) gtk_builder_get_object(builder, "aboutdialog");
}

GtkDialog* ui_renamedialog()
{
    return (GtkDialog*) gtk_builder_get_object(builder, "renamedialog");
}

GtkLabel* ui_renamedialog_label()
{
    return (GtkLabel*) gtk_builder_get_object(builder, "label");
}

GtkEntry* ui_renamedialog_entry()
{
    return (GtkEntry*) gtk_builder_get_object(builder, "entry");
}

GtkMessageDialog* ui_errordialog()
{
    GtkMessageDialog* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "An error occured");
    gtk_window_set_title(GTK_WINDOW(dialog), "pasystray");
    return dialog;
}
