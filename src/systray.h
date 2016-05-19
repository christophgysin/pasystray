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

#ifndef PASYSTRAY_SYSTRAY_H
#define PASYSTRAY_SYSTRAY_H

#include <gtk/gtk.h>

#include "menu_info.h"
#include "systray_impl.h"

void systray_create(menu_infos_t* mis);

void systray_menu_create(menu_infos_t* mis);
void systray_rootmenu_add_submenu(menu_infos_t* mis, menu_type_t type, const char* name, const char* icon);
void systray_menu_add_separator(GtkMenuShell* menu);
void systray_menu_add_application(GtkMenuShell* menu, const char* text, const char* icon, const char* command);
void systray_menu_add_action(GtkMenuShell* menu, const char* name, const char* icon, GCallback cb);

GtkWidget* systray_menu_add_item(GtkMenuShell* menu, const char* name, const char* icon, gboolean sensitive);
GtkWidget* systray_menu_add_submenu(GtkMenuShell* menu, menu_info_t* mis, const char* name, const char* tooltip, const char* icon);

void systray_add_placeholder(menu_info_t* mi);
void systray_remove_placeholder(menu_info_t* mi);

GtkWidget* systray_add_menu_item(menu_info_t* mi, const char* desc, const char* tooltip, const char* icon);
void systray_remove_menu_item(menu_info_t* mi, GtkWidget* item);

GtkWidget* systray_add_radio_item(menu_info_item_t* mii, const char* tooltip);
void systray_remove_radio_item(menu_info_t* mi, GtkWidget* item);

GtkWidget* systray_add_item(GtkMenuShell* menu, const char* desc, const char* tooltip, const char* icon);
void systray_remove_item(menu_info_item_t* mii);

void systray_add_all_items_to_submenu(menu_info_t* submenu, menu_info_item_t* item);
void systray_update_all_items_in_submenu(menu_info_t* submenu, menu_info_item_t* item);
void systray_remove_all_items_from_submenu(menu_info_t* submenu);
void systray_add_item_to_all_submenus(menu_info_item_t* item, menu_info_t* submenu);
void systray_update_item_in_all_submenus(menu_info_item_t* item, menu_info_t* submenu);
void systray_remove_item_from_all_submenus(menu_info_item_t* item, menu_info_t* submenu);

void systray_about_dialog();

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer userdata);
void systray_scroll_cb(guint state, GdkScrollDirection direction, menu_infos_t* mis);
void start_application_cb(GtkMenuItem* menuitem, const char* command);

void systray_set_tooltip(GtkWidget* item, const char* tooltip);

#endif /* PASYSTRAY_SYSTRAY_H */
