/***
  This file is part of PaSystray

  Copyright (C) 2011, 2012 Christoph Gysin

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

#include <gtk/gtk.h>

static GtkBuilder* builder;

void ui_load()
{
    builder = gtk_builder_new();

    GError* error = NULL;

    // GLADE_FILE is set in src/Makefile.am
    const char* filename = GLADE_FILE;

#ifdef DEBUG
    /* try to load ui in current dir first */
    char* local_file = "pasystray.glade";
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;
    local_file = "src/pasystray.glade";
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;

    g_message("using UI file: %s", filename);
#endif

    guint ret = gtk_builder_add_from_file(builder, filename, &error);

    if(!ret)
    {
        g_error("[ui] %s", error->message);
        g_error_free(error);
        return;
    }

    gtk_about_dialog_set_version(ui_aboutdialog(), PACKAGE_VERSION);
    gtk_about_dialog_set_website(ui_aboutdialog(), PACKAGE_URL);
    gtk_about_dialog_set_website_label(ui_aboutdialog(), PACKAGE_URL);
}

GtkStatusIcon* ui_statusicon()
{
    GObject* status_icon = gtk_builder_get_object(builder, "statusicon");

#ifdef DEBUG
    GError* error = NULL;
    gchar* exe_path = g_file_read_link("/proc/self/exe", &error);
    if(error)
    {
        g_message("g_file_read_link() failed (%s)", error->message);
        g_error_free(error);
        g_free(exe_path);
        return GTK_STATUS_ICON(status_icon);
    }

    gchar* dirname = g_path_get_dirname(exe_path);
    g_free(exe_path);

    gchar* icon_file = g_build_path("/", dirname, "../data/pasystray.svg", NULL);
    g_free(dirname);

    if(g_file_test(icon_file, G_FILE_TEST_EXISTS))
    {
        gtk_status_icon_set_from_file(GTK_STATUS_ICON(status_icon), icon_file);
        g_message("using icon: %s", icon_file);
    }
    g_free(icon_file);
#endif

    return GTK_STATUS_ICON(status_icon);
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
