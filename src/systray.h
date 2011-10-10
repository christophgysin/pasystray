#ifndef PASYSTRAY_SYSTRAY_H
#define PASYSTRAY_SYSTRAY_H

#include <gtk/gtk.h>

#include "menu_info.h"

void systray_create(menu_infos_t* mis);

void systray_menu_create(menu_infos_t* mis);
void systray_menu_add_submenu(GtkMenuShell* menu, const char* name, menu_info_t* m, const char* icon_name);
void systray_menu_add_separator(GtkMenuShell* menu);
void systray_menu_add_application(GtkMenuShell* menu, const char* text, const char* icon, const char* command);

GtkWidget* systray_submenu_add_radio_item(menu_info_t* m, const char* text);
GtkWidget* systray_submenu_add_menu_item(menu_info_t* m, const char* text, const char* icon);

GtkWidget* systray_menu_item_quit();
GtkWidget* systray_create_menuitem(GtkMenu* m, const char* text, const char* icon_name);

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer userdata);
void start_application_cb(GtkMenuItem* menuitem, const char* command);

#endif /* PASYSTRAY_SYSTRAY_H */
