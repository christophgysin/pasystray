#ifndef PASYSTRAY_MENU_INFO_H
#define PASYSTRAY_MENU_INFO_H

#include <gtk/gtk.h>
#include <stdint.h>

typedef struct {
    char* name;
    GtkMenuShell* menu;
    GSList* group;
    GHashTable* items;
} menu_info_t;

typedef struct {
    GtkMenuShell* menu;
    menu_info_t* servers;
    menu_info_t* sinks;
    menu_info_t* sources;
    menu_info_t* sink_inputs;
    menu_info_t* source_outputs;
} menu_infos_t;

typedef struct {
    char* desc;
    GtkWidget* widget;
} menu_info_item_t;

menu_infos_t* menu_infos_create();
void menu_infos_clear(menu_infos_t* mi);
void menu_infos_destroy(menu_infos_t* mi);

menu_info_t* menu_info_create(const char* name);
void menu_info_clear(menu_info_t* mi);
void menu_info_destroy(menu_info_t* i);

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* desc);
menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index);
void menu_info_item_remove(menu_info_t* mi, uint32_t index);
void menu_info_item_destroy(menu_info_item_t* mii);

#endif /* PASYSTRAY_MENU_INFO_H */
