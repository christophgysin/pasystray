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

    const char* filename = GLADE_FILE;

#ifdef DEBUG
    if(!g_file_test(filename, G_FILE_TEST_EXISTS))
        filename = g_strdup_printf("src/pasystray.glade");
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
    return (GtkStatusIcon*) gtk_builder_get_object(builder, "statusicon");
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
