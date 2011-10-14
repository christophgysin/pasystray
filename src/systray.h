#ifndef PASYSTRAY_SYSTRAY_H
#define PASYSTRAY_SYSTRAY_H

#include <gtk/gtk.h>

#include "menu_info.h"

void systray_create(menu_infos_t* mis);

void systray_menu_create(menu_infos_t* mis);
void systray_menu_add_submenu(menu_infos_t* mis, menu_type_t type, const char* name, const char* icon);
void systray_menu_add_separator(GtkMenuShell* menu);
void systray_menu_add_application(GtkMenuShell* menu, const char* text, const char* icon, const char* command);

GtkWidget* systray_add_menu_item(GtkMenuShell* menu, const char* desc, const char* tooltip, const char* icon);
void systray_remove_menu_item(menu_info_t* mi, GtkWidget* item);

GtkWidget* systray_add_radio_item(menu_info_t* m, const char* desc, const char* tooltip);
void systray_remove_radio_item(menu_info_t* mi, GtkWidget* item);

GtkWidget* systray_menu_item_quit();

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer userdata);
void start_application_cb(GtkMenuItem* menuitem, const char* command);

#endif /* PASYSTRAY_SYSTRAY_H */
