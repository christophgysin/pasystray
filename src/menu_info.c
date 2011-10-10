#include <glib.h>

#include "menu_info.h"
#include "systray.h"

menu_infos_t* menu_infos_create()
{
    menu_infos_t* mi = g_new(menu_infos_t, 1);

    mi->servers = menu_info_create("server");
    mi->sinks = menu_info_create("sink");
    mi->sources = menu_info_create("source");
    mi->sink_inputs = menu_info_create("input");
    mi->source_outputs = menu_info_create("output");

    return mi;
}

void menu_infos_clear(menu_infos_t* mi)
{
    menu_info_clear(mi->servers);
    menu_info_clear(mi->sinks);
    menu_info_clear(mi->sources);
    menu_info_clear(mi->sink_inputs);
    menu_info_clear(mi->source_outputs);
}

void menu_infos_destroy(menu_infos_t* mi)
{
    menu_info_destroy(mi->servers);
    menu_info_destroy(mi->sinks);
    menu_info_destroy(mi->sources);
    menu_info_destroy(mi->sink_inputs);
    menu_info_destroy(mi->source_outputs);

    g_free(mi);
}

menu_info_t* menu_info_create(const char* name)
{
    menu_info_t* mi = g_new(menu_info_t, 1);

    mi->name = g_strdup(name);
    mi->menu = NULL;
    mi->group = NULL;
    mi->items = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify) menu_info_item_destroy);

    return mi;
}

void menu_info_clear(menu_info_t* mi)
{
    g_hash_table_remove_all(mi->items);
}

void menu_info_destroy(menu_info_t* i)
{
    g_free(i->name);
    //g_free(i->items);

    g_free(i);
}

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* desc)
{
    menu_info_item_t* item = g_new(menu_info_item_t, 1);

    fprintf(stderr, "HT: adding %s %u %p\n", mi->name, index, item);

    item->desc = g_strdup(desc);
    item->widget = systray_submenu_add_radio_item(mi, desc);

    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), item);
}

menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index)
{
    fprintf(stderr, "HT: lookup %s %u\n", mi->name, index);
    menu_info_item_t* item = g_hash_table_lookup(mi->items, GUINT_TO_POINTER(index));

    if(!item)
        fprintf(stderr, "HT: %s %u not found!\n", mi->name, index);

    return item;
}

void menu_info_item_remove(menu_info_t* mi, uint32_t index)
{
    fprintf(stderr, "HT: trying to remove %s %u\n", mi->name, index);

    menu_info_item_t* mii = menu_info_item_get(mi, index);

    if(!mii)
        return;

    fprintf(stderr, "HT: removing %s %u\n", mi->name, index);

    gtk_container_remove(GTK_CONTAINER(mi->menu), mii->widget);
    g_hash_table_remove(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_destroy(menu_info_item_t* mii)
{
    g_free(mii->desc);
    g_free(mii);
}
