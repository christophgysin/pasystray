/***
  This file is part of PaSystray

  Copyright (C) 2011-2016  Christoph Gysin

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

#include <string.h>
#include <glib.h>

#include "menu_info.h"
#include "systray.h"
#include "pulseaudio_action.h"
#include "ui.h"
#include "x11-property.h"

menu_infos_t* menu_infos_create()
{
    menu_infos_t* mis = g_new(menu_infos_t, 1);

    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
        menu_info_init(mis, &mis->menu_info[i], i);

    return mis;
}

void menu_infos_init(menu_infos_t* mis)
{
    menu_info_t* servers = &mis->menu_info[MENU_SERVER];
    servers->default_name = x11_property_get("PULSE_SERVER");

    if(servers->default_name)
        menu_info_item_update(servers, -1, servers->default_name,
                servers->default_name, NULL, 0, "detected in X properties",
                NULL, servers->default_name, -1);
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
                case MENU_MODULE:
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
    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
        menu_info_destroy(&mis->menu_info[i]);

    g_free(mis);
}

menu_info_t* menu_info_create(menu_infos_t* mis, menu_type_t type)
{
    menu_info_t* mi = g_new(menu_info_t, 1);
    menu_info_init(mis, mi, type);
    return mi;
}

void menu_info_init(menu_infos_t* mis, menu_info_t* mi, menu_type_t type)
{
    mi->type = type;
    mi->menu = NULL;
    mi->group = NULL;

    GHashFunc hash_func = g_direct_hash;
    GEqualFunc eq_func = g_direct_equal;

    if(type == MENU_SERVER)
    {
        hash_func = g_str_hash;
        eq_func = g_str_equal;
    }

    mi->items = g_hash_table_new_full(hash_func, eq_func, NULL,
            (GDestroyNotify) menu_info_item_destroy);

    mi->menu_infos = mis;
    mi->parent = NULL;

    mi->default_name = NULL;
}

void menu_info_destroy(menu_info_t* mi)
{
    g_hash_table_destroy(mi->items);
    g_free(mi->default_name);
}

void menu_info_item_init(menu_info_item_t* mii)
{
    mii->name = NULL;
    mii->desc = NULL;
    mii->volume = NULL;
    mii->notify = NULL;
    mii->icon = NULL;
    mii->widget = NULL;
    mii->menu_info = NULL;
    mii->submenu = NULL;
    mii->context = NULL;
    mii->address = NULL;
}

void menu_info_item_destroy(menu_info_item_t* mii)
{
    g_free(mii->name);
    g_free(mii->desc);
    g_free(mii->volume);
    g_free(mii->icon);
    g_free(mii->address);
    g_free(mii);
}

const char* menu_info_type_name(menu_type_t type)
{
    static const char* MENU_NAME[] = {
        [MENU_SERVER] = "server",
        [MENU_SINK]   = "sink",
        [MENU_SOURCE] = "source",
        [MENU_INPUT]  = "input",
        [MENU_OUTPUT] = "output",
        [MENU_MODULE] = "module",
    };

    return MENU_NAME[type];
}

menu_type_t menu_info_submenu_type(menu_type_t menu_type)
{
    switch(menu_type)
    {
        /* no submenu, return self */
        case MENU_SERVER:
        case MENU_MODULE:
            return menu_type;

        case MENU_SINK:
            return MENU_INPUT;
        case MENU_SOURCE:
            return MENU_OUTPUT;
        case MENU_INPUT:
            return MENU_SINK;
        case MENU_OUTPUT:
            return MENU_SOURCE;
    }
    return -1;
}

char* menu_info_item_label(menu_info_item_t* mii)
{
    gchar* desc = mii->desc;

    const size_t DESC_MAX = 80;
    gchar desc_buf[DESC_MAX + 1];
    if (strlen(mii->desc) > DESC_MAX)
    {
        g_snprintf(desc_buf, DESC_MAX+1, "%.*s...", (int)DESC_MAX-3, mii->desc);
        desc = desc_buf;
    }

    if(!mii->volume)
        return g_strdup(desc);

    char vol_buf[PA_CVOLUME_SNPRINT_MAX];
    gchar* label = g_strdup_printf("%s %s%s",
        desc,
        pa_volume_snprint(vol_buf, sizeof(vol_buf), mii->volume->values[0]),
        mii->mute ? " [muted]" : "");

    return label;
}

void menu_info_item_update(menu_info_t* mi, uint32_t index, const char* name,
        const char* desc, const pa_cvolume* vol, int mute, char* tooltip,
        const char* icon, const char* address, uint32_t target)
{
    menu_info_item_t* item;

    if(mi->type == MENU_SERVER)
        item = menu_info_item_get_by_name(mi, name);
    else
        item = menu_info_item_get(mi, index);

    if(item == NULL)
    {
        menu_info_item_add(mi, index, name, desc, vol, mute, tooltip, icon, address, target);
        return;
    }

    g_debug("[menu_info] updating %s %u %s (target: %d)",
            menu_info_type_name(item->menu_info->type), index, desc, (int)target);

    if (name != item->name)
    {
        g_free(item->name);
        item->name = g_strdup(name);
    }

    if (desc != item->desc)
    {
        g_free(item->desc);
        item->desc = g_strdup(desc);
    }

    /* only notify on volume / mute changes */
    int notify = 0;
    if ((vol && !pa_cvolume_equal(item->volume, vol)) || mute != item->mute)
        notify = 1;

    if (vol != item->volume)
    {
        g_free(item->volume);
        item->volume = g_memdup2(vol, sizeof(pa_cvolume));
    }
    item->mute = mute;
    item->target = target;

    if (address != item->address)
    {
        g_free(item->address);
        item->address = g_strdup(address);
    }

    menu_type_t submenu_type = menu_info_submenu_type(mi->type);
    menu_info_t* submenu = &mi->menu_infos->menu_info[submenu_type];

    gchar* label = menu_info_item_label(item);
    gtk_menu_item_set_label(GTK_MENU_ITEM(item->widget), label);
    g_free(label);

    if (tooltip)
        systray_set_tooltip(GTK_WIDGET(item->widget), tooltip);

    switch(mi->type)
    {
        case MENU_SERVER:
        case MENU_MODULE:
            g_debug("[menu_info] *** unhandled %s update! (index: %u, desc: %s)",
                    menu_info_type_name(mi->type), index, item->desc);
            break;
        case MENU_SINK:
        case MENU_SOURCE:
            ui_set_volume_icon(item); // update sink item
            systray_update_item_in_all_submenus(item, submenu);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            systray_update_all_items_in_submenu(submenu, item);
            break;
    }

    /* if this is the default sink, update status icon acording to volume */
    if(mi->type == MENU_SINK && item == menu_info_item_get_by_name(mi, mi->default_name))
        ui_update_systray_icon(item);

    if(notify)
        pulseaudio_process_update_volume_notification(item);
}

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* name,
        const char* desc, const pa_cvolume* vol, int mute, char* tooltip,
        const char* icon, const char* address, uint32_t target)
{
    menu_infos_t* mis = mi->menu_infos;
    menu_info_item_t* item = g_new(menu_info_item_t, 1);
    menu_info_item_init(item);
    item->menu_info = mi;

    g_debug("[menu_info] adding %s %u %s (target: %d)",
            menu_info_type_name(mi->type), index, desc, (int)target);

    item->index = index;
    item->name = g_strdup(name);
    item->desc = g_strdup(desc);
    item->volume = g_memdup2(vol, sizeof(pa_cvolume));
    item->target = target;
    item->mute = mute;
    item->icon = g_strdup(icon);
    item->address = g_strdup(address);

    menu_type_t submenu_type = menu_info_submenu_type(mi->type);
    menu_info_t* submenu = &mis->menu_info[submenu_type];

    switch(item->menu_info->type)
    {
        case MENU_INPUT:
        case MENU_OUTPUT:
            item->submenu = menu_info_create(mis, submenu_type);
            item->submenu->parent = item;
            break;
        default:
            break;
    }

    switch(mi->type)
    {
        case MENU_SERVER:
            item->widget = systray_add_radio_item(item, tooltip);
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item->widget),
                    (item->address == mi->default_name) ||
                    (item->address && mi->default_name &&
                     g_str_equal(mi->default_name, item->address)));
            break;
        case MENU_SINK:
        case MENU_SOURCE:
            item->context = menu_info_item_context_menu(item);
            item->widget = systray_add_radio_item(item, tooltip);
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item->widget),
                    g_str_equal(mi->default_name, item->name));
            systray_add_item_to_all_submenus(item, submenu);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
        {
            gchar* label = menu_info_item_label(item);
            item->widget = systray_menu_add_submenu(mi->menu, item->submenu,
                    label, tooltip, icon);
            g_free(label);
            systray_add_all_items_to_submenu(submenu, item);
            break;
        }
        case MENU_MODULE:
            item->context = menu_info_item_context_menu(item);
            item->widget = systray_add_menu_item(mi, item->desc, tooltip, icon);
            break;
    }

    g_signal_connect(item->widget, "activate",
            G_CALLBACK(menu_info_item_activated), item);
    g_signal_connect(item->widget, "button-press-event",
            G_CALLBACK(menu_info_item_clicked), item);
    gtk_widget_add_events(item->widget, GDK_SCROLL_MASK);
    g_signal_connect(item->widget, "scroll-event",
            G_CALLBACK(menu_info_item_scrolled), item);

    if(mi->type == MENU_SERVER)
        g_hash_table_insert(mi->items, item->name, item);
    else
        g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), item);

    /* if this is the default sink, update status icon acording to volume */
    if(mi->type == MENU_SINK && item == menu_info_item_get_by_name(mi, mi->default_name))
        ui_update_systray_icon(item);
}

GtkMenuShell* menu_info_item_context_menu(menu_info_item_t* mii)
{
    GtkMenuShell* menu = GTK_MENU_SHELL(gtk_menu_new());
    GtkWidget* item = NULL;

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
        case MENU_INPUT:
        case MENU_OUTPUT:
            break;

        case MENU_SINK:
        case MENU_SOURCE:
        {
            menu_type_t type = (mii->menu_info->type == MENU_SINK) ? MENU_INPUT : MENU_OUTPUT;
            gchar* label = g_strdup_printf("Move all %ss here",
                    menu_info_type_name(type));
            item = gtk_menu_item_new_with_label(label);
            g_free(label);
            g_signal_connect(item, "button-press-event",
                    G_CALLBACK(menu_info_item_move_all_cb), mii);
            gtk_menu_shell_append(menu, item);

            item = gtk_menu_item_new_with_label("Rename");
            g_signal_connect(item, "button-press-event",
                    G_CALLBACK(menu_info_item_rename_cb), mii);
            gtk_menu_shell_append(menu, item);
            break;
        }
        case MENU_MODULE:
            item = gtk_menu_item_new_with_label("Unload");
            g_signal_connect(item, "button-press-event",
                    G_CALLBACK(menu_info_module_unload_cb), mii);
            gtk_menu_shell_append(menu, item);
            break;
    }

    gtk_widget_show_all(GTK_WIDGET(menu));
    return menu;
}

void menu_info_subitem_add(menu_info_t* mi, uint32_t index, const char* name,
        const char* desc, char* tooltip, const char* icon)
{
    menu_info_item_t* subitem = g_new(menu_info_item_t, 1);
    menu_info_item_init(subitem);
    subitem->index = index;
    subitem->name = g_strdup(name);
    subitem->desc = g_strdup(desc);
    subitem->menu_info = mi;
    subitem->widget = systray_add_radio_item(subitem, tooltip);

    gboolean active = mi->parent->target == index;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(subitem->widget), active);

    g_debug("[menu_info] adding subitem %s %u '%s' %s", menu_info_type_name(mi->type),
            index, desc, active ? " (active)" : "");

    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), subitem);

    g_signal_connect(subitem->widget, "button-press-event",
            G_CALLBACK(menu_info_subitem_clicked), subitem);

    g_signal_connect(subitem->widget, "activate",
            G_CALLBACK(menu_info_subitem_activated), subitem);
}

void menu_info_subitem_update(menu_info_t* mi, uint32_t index, const char* name,
        const char* desc, char* tooltip, const char* icon)
{
    menu_info_item_t* item = menu_info_item_get(mi, index);

   if(item == NULL)
       g_error("[menu_info] subitem not found!");

    gboolean active = mi->parent->target == index;

    g_debug("[menu_info] updating subitem %s %u '%s' %s", menu_info_type_name(mi->type),
            index, desc, active ? " (active)" : "");

    gchar* label = menu_info_item_label(item);
    gtk_menu_item_set_label(GTK_MENU_ITEM(item->widget), label);
    g_free(label);

    if(active)
        menu_info_subitem_set_active(item, TRUE);
}

menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index)
{
    return g_hash_table_lookup(mi->items, GUINT_TO_POINTER(index));
}

gboolean name_equal(gpointer key, gpointer value, gpointer user_data)
{
    menu_info_item_t* mii = value;
    const char* name = user_data;
    return g_str_equal(mii->name, name);
}

menu_info_item_t* menu_info_item_get_by_name(menu_info_t* mi, const char* name)
{
    return g_hash_table_find(mi->items, name_equal, (gpointer)name);
}

gboolean desc_equal(gpointer key, gpointer value, gpointer user_data)
{
    menu_info_item_t* mii = value;
    const char* desc = user_data;
    return g_str_equal(mii->desc, desc);
}

menu_info_item_t* menu_info_item_get_by_desc(menu_info_t* mi, const char* desc)
{
    return g_hash_table_find(mi->items, desc_equal, (gpointer)desc);
}

void menu_info_item_set_active(menu_info_item_t* mii, gboolean is_active)
{
    g_signal_handlers_block_by_func(mii->widget, G_CALLBACK(menu_info_item_activated), mii);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mii->widget), is_active);
    g_signal_handlers_unblock_by_func(mii->widget, G_CALLBACK(menu_info_item_activated), mii);
}

void menu_info_subitem_set_active(menu_info_item_t* mii, gboolean is_active)
{
    g_signal_handlers_block_by_func(mii->widget, G_CALLBACK(menu_info_subitem_activated), mii);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mii->widget), is_active);
    g_signal_handlers_unblock_by_func(mii->widget, G_CALLBACK(menu_info_subitem_activated), mii);
}

void menu_info_item_activated(GtkWidget* item, menu_info_item_t* mii)
{
    if (!GTK_IS_CHECK_MENU_ITEM(item)
            || !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
    {
        /* Ignore activation of submenus and deselected items */
        return;
    }

    GdkEvent *current_event = gtk_get_current_event();
    if (current_event) {
        GdkEventType type = current_event->type;
        gdk_event_free(current_event);

        /* Ignore activation if already handled by clicked callback */
        if (type == GDK_BUTTON_PRESS || type == GDK_BUTTON_RELEASE)
            return;
    }

    g_debug("[menu_info] item activated: %s %s",
            menu_info_type_name(mii->menu_info->type),
            mii->name);

    /* on left-click, set device as default */
    pulseaudio_set_default(mii);
}

void menu_info_item_clicked(GtkWidget* item, GdkEventButton* event,
        menu_info_item_t* mii)
{
    g_debug("[menu_info] item clicked mod:%s%s button:%i",
            (event->state & GDK_CONTROL_MASK) ? "[ctrl]" : "",
            (event->state & GDK_MOD1_MASK) ? "[alt]" : "",
            event->button);

    switch(event->button)
    {
        case 1:
            /* on alt + left-click, toggle mute device/stream */
            if(event->state & GDK_MOD1_MASK)
                pulseaudio_toggle_mute(mii);

            /* on left-click, set device as default */
            else
                pulseaudio_set_default(mii);
            break;
        /* on middle-click, toggle mute device/stream */
        case 2:
            pulseaudio_toggle_mute(mii);
            break;
        /* on right-click, show context menu (if any) */
        case 3:
            if(mii->context)
                gtk_menu_popup(GTK_MENU(mii->context), NULL, NULL, NULL, NULL,
                        (event != NULL) ? event->button : 0,
                        gdk_event_get_time((GdkEvent*)event));
            break;
    }
}

void menu_info_item_scrolled(GtkWidget* item, GdkEventScroll* event,
        menu_info_item_t* mii)
{
    g_debug("[menu_info] scroll-event mod:%s dir:%s",
            (event->state & GDK_CONTROL_MASK) ? "ctrl" : "",
            (event->direction == GDK_SCROLL_UP) ? "up" :
            (event->direction == GDK_SCROLL_DOWN) ? "down" :
            (event->direction == GDK_SCROLL_LEFT) ? "left" :
            (event->direction == GDK_SCROLL_RIGHT) ? "right" : "???");

    menu_infos_t* mis = mii->menu_info->menu_infos;

    int inc = 0;

    switch(event->direction)
    {
        case GDK_SCROLL_UP:
            inc = mis->settings.volume_inc;
            break;
        case GDK_SCROLL_DOWN:
            inc = -mis->settings.volume_inc;
            break;
        default:
            return;
    }

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
        case MENU_MODULE:
            break;
        case MENU_SINK:
        case MENU_SOURCE:
        case MENU_INPUT:
        case MENU_OUTPUT:
            pulseaudio_volume(mii, inc);
            break;
    }
}

void menu_info_subitem_activated(GtkWidget* item, menu_info_item_t* mii) {

    if (!GTK_IS_CHECK_MENU_ITEM(item)
            || !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
    {
        /* Ignore activation of submenus and deselected items */
        return;
    }

    g_debug("[menu_info] subitem activated: %s %s",
            menu_info_type_name(mii->menu_info->type),
            mii->name);

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
        case MENU_INPUT:
        case MENU_OUTPUT:
        case MENU_MODULE:
            break;
        case MENU_SINK:
            pulseaudio_move_input_to_sink(mii->menu_info->parent, mii);
            break;
        case MENU_SOURCE:
            pulseaudio_move_output_to_source(mii->menu_info->parent, mii);
            break;
    }
}

void menu_info_subitem_clicked(GtkWidget* item, GdkEventButton* event,
        menu_info_item_t* mii)
{
    g_debug("[menu_info] subitem clicked mod:%s%s button:%i",
            (event->state & GDK_CONTROL_MASK) ? "[ctrl]" : "",
            (event->state & GDK_MOD1_MASK) ? "[alt]" : "",
            event->button);
}

void menu_info_item_move_all_cb(GtkWidget* item, GdkEventButton* event, void* userdata)
{
    menu_info_item_t* mii = userdata;
    pulseaudio_move_all(mii);
}

void menu_info_item_rename_cb(GtkWidget* item, GdkEventButton* event, void* userdata)
{
    menu_info_item_t* mii = userdata;
    menu_info_item_rename_dialog(mii);
}

void menu_info_item_rename_dialog(menu_info_item_t* mii)
{
    gtk_menu_popdown(GTK_MENU(mii->menu_info->menu_infos->menu));

    char* title = g_strdup_printf("Rename %s %s",
            menu_info_type_name(mii->menu_info->type), mii->name);
    char* text = g_strdup_printf("%s to:", title);

    GtkDialog* dialog = ui_renamedialog();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_label_set_text(ui_renamedialog_label(), text);
    gtk_entry_set_text(ui_renamedialog_entry(), mii->desc);

    g_free(text);
    g_free(title);

    gtk_widget_show_all(GTK_WIDGET(dialog));

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
    {
        const char* entry = gtk_entry_get_text(GTK_ENTRY(ui_renamedialog_entry()));
        char* name = g_strstrip(g_strdup(entry));

        if(!g_str_equal(name, mii->desc))
            pulseaudio_rename(mii, name);

        g_free(name);
    }

    gtk_widget_hide(GTK_WIDGET(dialog));
}

void menu_info_item_rename_error(menu_info_item_t* mii)
{
    gchar* title = g_strdup_printf("Failed to rename %s",
            menu_info_type_name(mii->menu_info->type));
    GtkDialog* dialog = ui_errordialog(title,
            "Is the module-device-manager loaded?");
    g_free(title);

    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void menu_info_module_unload_cb(GtkWidget* item, GdkEventButton* event, void* userdata)
{
    pulseaudio_module_unload(userdata);
}

void menu_info_item_remove(menu_info_t* mi, uint32_t index)
{
    menu_info_item_t* mii = menu_info_item_get(mi, index);

    if(!mii)
        return;

    g_debug("[menu_info] removing %s %u",
            menu_info_type_name(mi->type), index);

    systray_remove_item(mii);

    g_hash_table_remove(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_remove_by_name(menu_info_t* mi, const char* name)
{
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, name);

    if(!mii)
    {
        g_warning("failed to remove %s '%s': item not found!",
                menu_info_type_name(mi->type), name);
        return;
    }

    g_debug("[menu_info] removing %s %s (by name: '%s')",
            menu_info_type_name(mi->type), mii->desc, name);

    systray_remove_item(mii);

    g_hash_table_foreach_remove(mi->items, name_equal, mii->name);
}
