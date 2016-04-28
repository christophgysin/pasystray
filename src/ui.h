/***
  This file is part of PaSystray

  Copyright (C) 2011-2015  Christoph Gysin

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

#ifndef PASYSTRAY_UI_H
#define PASYSTRAY_UI_H

#include "menu_info.h"
#include <gtk/gtk.h>

void ui_load();
void ui_set_volume_icon(menu_info_item_t* mii);
void ui_update_systray_icon(menu_info_item_t* mii);
GtkDialog* ui_aboutdialog();
GtkDialog* ui_renamedialog();
GtkLabel* ui_renamedialog_label();
GtkEntry* ui_renamedialog_entry();
GtkDialog* ui_errordialog(const gchar* title, const gchar* message);

#endif /* PASYSTRAY_UI_H */
