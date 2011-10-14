#include <glib.h>

#include "menu_info.h"
#include "systray.h"
#include "pulseaudio_action.h"

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
        menu_info_t* mi = &mis->menu_info[i];

        GHashTableIter iter;
        gpointer key;
        menu_info_item_t* mii;

        g_hash_table_iter_init(&iter, mi->items);

        while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        {
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
            g_hash_table_iter_remove(&iter);
        }
        mis->menu_info[i].group = NULL;
    }
}

void menu_infos_destroy(menu_infos_t* mis)
{
    g_free(mis);
}

const char* menu_info_type_name(menu_type_t type)
{
    static const char* MENU_NAME[] = {
        [MENU_SERVER] = "server",
        [MENU_SINK]   = "sink",
        [MENU_SOURCE] = "source",
        [MENU_INPUT]  = "input",
        [MENU_OUTPUT] = "output",
    };

    return MENU_NAME[type];
}

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, char* tooltip, const char* icon)
{
    menu_info_item_t* item = g_new(menu_info_item_t, 1);
    item->menu_info = mi;

#ifdef DEBUG
    fprintf(stderr, "[menu_info] adding %s %u %p\n", menu_info_type_name(mi->type), index, item);
#endif

    item->name = g_strdup(name);
    item->desc = g_strdup(desc);
    item->icon = g_strdup(icon);

    switch(mi->type)
    {
        case MENU_SERVER:
        case MENU_SINK:
        case MENU_SOURCE:
            item->widget = systray_add_radio_item(mi, desc, tooltip);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            item->widget = systray_add_menu_item(mi->menu, desc, tooltip, icon);
            break;
    }

    g_signal_connect(item->widget, "activate", G_CALLBACK(menu_info_item_activated), item);
    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), item);
}

menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index)
{
    return g_hash_table_lookup(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_activated(GtkMenuItem* item, gpointer userdata)
{
    menu_info_item_t* mii = (menu_info_item_t*) userdata;

    /* filter signal "activate" from radio items that got deselected */
    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
        case MENU_SINK:
        case MENU_SOURCE:
            if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
                return;
        default:
            break;
    }
    menu_info_item_clicked(mii);
}

void menu_info_item_clicked(menu_info_item_t* mii)
{
#ifdef DEBUG
    fprintf(stderr, "clicked %s %s (%s)\n", menu_info_type_name(mii->menu_info->type), mii->desc, mii->name);
#endif

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            /* TODO: connect to different server */
            break;
        case MENU_SINK:
            pulseaudio_set_sink(mii);
            break;
        case MENU_SOURCE:
            pulseaudio_set_source(mii);
            break;
        case MENU_INPUT:
            /* TODO: move input to different sink */
            break;
        case MENU_OUTPUT:
            /* TODO: move output to different source */
            break;
    }
}

void menu_info_item_remove(menu_infos_t* mis, menu_type_t type, uint32_t index)
{
    menu_info_t* mi = &mis->menu_info[type];
    menu_info_item_t* mii = menu_info_item_get(mi, index);

    if(!mii)
        return;

#ifdef DEBUG
    fprintf(stderr, "[menu_info] removing %s %u\n", menu_info_type_name(type), index);
#endif

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
