#include <glib.h>

#include "menu_info.h"
#include "systray.h"

static const char* MENU_NAME[] = {
    [MENU_SERVER] = "server",
    [MENU_SINK]   = "sink",
    [MENU_SOURCE] = "source",
    [MENU_INPUT]  = "input",
    [MENU_OUTPUT]  = "output",
};

menu_infos_t* menu_infos_create()
{
    menu_infos_t* mis = g_new(menu_infos_t, 1);

    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
    {
        mis->menu_info[i].type = i;
        mis->menu_info[i].menu = NULL;
        mis->menu_info[i].group = NULL;
        mis->menu_info[i].items = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)menu_info_item_destroy);
    }

    return mis;
}

void menu_infos_clear(menu_infos_t* mis)
{
    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
    {
        g_hash_table_remove_all(mis->menu_info[i].items);
        mis->menu_info[i].group = NULL;
    }
}

void menu_infos_destroy(menu_infos_t* mis)
{
    g_free(mis);
}

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, const char* icon)
{
    menu_info_item_t* item = g_new(menu_info_item_t, 1);

    fprintf(stderr, "[menu_info] adding %s %u %p\n", MENU_NAME[mi->type], index, item);

    item->name = g_strdup(name);
    item->icon = g_strdup(icon);

    switch(mi->type)
    {
        case MENU_SERVER:
        case MENU_SINK:
        case MENU_SOURCE:
            item->widget = systray_add_radio_item(mi, name, desc);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            item->widget = systray_add_menu_item(mi->menu, name, desc, icon);
            break;
    }

    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), item);
}

menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index)
{
    return g_hash_table_lookup(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_remove(menu_infos_t* mis, menu_type_t type, uint32_t index)
{
    menu_info_t* mi = &mis->menu_info[type];
    menu_info_item_t* mii = menu_info_item_get(mi, index);

    if(!mii)
        return;

    fprintf(stderr, "[menu_info] removing %s %u\n", MENU_NAME[type], index);

    switch(mi->type)
    {
        case MENU_SERVER:
        case MENU_SINK:
        case MENU_SOURCE:
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            systray_remove_menu_item(mi, mii->widget);
            break;
    }

    g_hash_table_remove(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_destroy(menu_info_item_t* mii)
{
    g_free(mii->name);
    g_free(mii->icon);
    g_free(mii);
}
