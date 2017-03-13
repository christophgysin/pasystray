/***
  This file is part of PaSystray

  Copyright (C) 2011-2016  Christoph Gysin

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

#include <glib.h>
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
    gchar* local_file = g_strdup_printf("pasystray.gtk%u.glade", GTK_VERSION_MAJOR);
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;
    gchar* local_file_src = g_strdup_printf("src/pasystray.gtk%u.glade", GTK_VERSION_MAJOR);
    if(g_file_test(local_file_src, G_FILE_TEST_EXISTS))
        filename = local_file_src;

    g_debug("using UI file: %s", filename);

    guint ret = gtk_builder_add_from_file(builder, filename, &error);

    g_free(local_file);
    g_free(local_file_src);

    if(!ret)
    {
        g_error("[ui] %s", error->message);
    }
}

void ui_set_volume_icon(menu_info_item_t* mii)
{
    g_debug("pulseaudio_set_volume_icon(%s)", mii->name);

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

    g_free(mii->icon);
    mii->icon = g_strdup(icon_name);
}

void ui_update_systray_icon(menu_info_item_t* mii)
{
    g_debug("pulseaudio_update_systray_icon(%s)", mii->name);

    ui_set_volume_icon(mii);

    menu_infos_t* mis = mii->menu_info->menu_infos;
    systray_impl_set_icon(mis->systray, mii->icon);
}

GtkDialog* ui_aboutdialog()
{
    GtkAboutDialog* aboutdialog = (GtkAboutDialog*) gtk_builder_get_object(builder, "aboutdialog");

    gtk_about_dialog_set_version(aboutdialog, PACKAGE_VERSION);
    gtk_about_dialog_set_website(aboutdialog, PACKAGE_URL);
    gtk_about_dialog_set_website_label(aboutdialog, PACKAGE_URL);

    return GTK_DIALOG(aboutdialog);
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

GtkDialog* ui_errordialog(const gchar* title, const gchar* message)
{
    GtkMessageDialog* dialog = (GtkMessageDialog*) gtk_builder_get_object(builder, "errordialog");

    gtk_message_dialog_set_markup(dialog, title);

    if(message)
        gtk_message_dialog_format_secondary_text(dialog, "%s", message);

    return GTK_DIALOG(dialog);
}
