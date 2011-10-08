#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <gtk/gtk.h>

typedef struct {
    GtkMenuShell* menu;
    GSList* group;
    GHashTable* items;
} menu_info_t;

typedef struct {
    GtkStatusIcon *icon;
    GtkWidget* menu;
    menu_info_t* servers;
    menu_info_t* sinks;
    menu_info_t* sources;
    menu_info_t* sink_inputs;
    menu_info_t* source_outputs;
} userdata_t;

extern userdata_t* u;

void systray_create();
void systray_create_gui();

void systray_menu_create();
void systray_menu_add_submenu(GtkWidget* menu, const char* name, menu_info_t* m, const char* icon_name);
void systray_menu_add_separator();
void systray_menu_add_application(const char* text, const char* icon, const char* command);

void systray_submenu_add_radio_item(menu_info_t* m, const char* text);
void systray_submenu_add_menu_item(menu_info_t* m, const char* text, const char* icon);

GtkWidget* systray_menu_item_quit();
GtkWidget* systray_create_menuitem(GtkMenu* m, const char* text, const char* icon_name);

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer user_data);
void start_application_cb(GtkMenuItem* menuitem, const char* command);

#endif /* SYSTRAY_H */
