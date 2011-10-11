#ifndef PASYSTRAY_MENU_INFO_H
#define PASYSTRAY_MENU_INFO_H

#include <gtk/gtk.h>
#include <stdint.h>

typedef enum {
    MENU_SERVER = 0,
    MENU_SINK = 1,
    MENU_SOURCE = 2,
    MENU_INPUT = 3,
    MENU_OUTPUT = 4,
} menu_type_t;
#define MENU_COUNT 5

typedef struct {
    menu_type_t type;
    GtkMenuShell* menu;
    GSList* group;
    GHashTable* items;
} menu_info_t;

typedef struct {
    GtkMenuShell* menu;
    menu_info_t menu_info[MENU_COUNT];
} menu_infos_t;

typedef struct {
    char* desc;
    char* icon;
    GtkWidget* widget;
} menu_info_item_t;

menu_infos_t* menu_infos_create();
void menu_infos_clear(menu_infos_t* mi);
void menu_infos_destroy(menu_infos_t* mi);

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* desc, const char* icon);
menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index);
void menu_info_item_remove(menu_infos_t* mis, menu_type_t type, uint32_t index);
void menu_info_item_destroy(menu_info_item_t* mii);

#endif /* PASYSTRAY_MENU_INFO_H */
