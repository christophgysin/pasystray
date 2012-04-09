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

#define GLADE_FILE "pasystray.glade"

static GtkBuilder* builder;

void ui_load()
{
    builder = gtk_builder_new();

    GError* error = NULL;
    guint ret = gtk_builder_add_from_file(builder, GLADE_FILE, &error);

    if(!ret)
    {
        g_error("[ui] failed to load %s: %s", GLADE_FILE, error->message);
        g_error_free(error);
        return;
    }

    gtk_about_dialog_set_version(ui_aboutdialog(), PACKAGE_VERSION);
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
