/***
  This file is part of PaSystray

  Copyright (C) 2011, 2012 Christoph Gysin

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

#include "systray.h"

#include "pasystray.h"
#include "config.h"
#include "pulseaudio_action.h"
#include "ui.h"
#include "systray_impl.h"

void systray_create(menu_infos_t* mis)
{
    systray_menu_create(mis);
    mis->systray = systray_impl_create(mis);
}

void systray_menu_create(menu_infos_t* mis)
{
    mis->menu = GTK_MENU_SHELL(gtk_menu_new());

    systray_rootmenu_add_submenu(mis, MENU_SERVER, "Default _Server", "network-wired");
    systray_rootmenu_add_submenu(mis, MENU_SINK, "Default S_ink", "audio-card");
    systray_rootmenu_add_submenu(mis, MENU_SOURCE, "Default S_ource", "audio-input-microphone");
    systray_rootmenu_add_submenu(mis, MENU_INPUT, "Playback S_treams", "player_play");
    systray_rootmenu_add_submenu(mis, MENU_OUTPUT, "Recording Stre_ams", "player_record");
    systray_menu_add_separator(mis->menu);

    static const char* COMMAND_PAMAN = "paman";
    static const char* COMMAND_PAVUCONTROL = "pavucontrol";
    static const char* COMMAND_PAVUMETER = "pavumeter";
    static const char* COMMAND_PAVUMETER_REC = "pavumeter --record";
    static const char* COMMAND_PAPREFS = "paprefs";

    systray_menu_add_application(mis->menu, "_Manager...", NULL, COMMAND_PAMAN);
    systray_menu_add_application(mis->menu, "Volume _Control...", NULL, COMMAND_PAVUCONTROL);
    systray_menu_add_application(mis->menu, "Volume Meter (_Playback)...", NULL, COMMAND_PAVUMETER);
    systray_menu_add_application(mis->menu, "Volume Meter (_Recording)...", NULL, COMMAND_PAVUMETER_REC);
    systray_menu_add_application(mis->menu, "Con_figure Local Sound Server...", NULL, COMMAND_PAPREFS);

    /* TODO: settings
    systray_menu_add_separator(mis->menu);
    item = append_menuitem(mis->menu, "_Preferences...", "gtk-preferences");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(show_preferences), NULL);
    */

    systray_menu_add_separator(mis->menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(mis->menu), systray_menu_item_about());
    gtk_menu_shell_append(GTK_MENU_SHELL(mis->menu), systray_menu_item_quit());
}

void systray_menu_add_separator(GtkMenuShell* menu)
{
    GtkWidget* item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(menu, item);
    gtk_widget_show(item);
}

void systray_rootmenu_add_submenu(menu_infos_t* mis, menu_type_t type, const char* name, const char* icon)
{
    GtkMenuShell* menu = mis->menu;
    menu_info_t* mi = &mis->menu_info[type];

    systray_menu_add_submenu(menu, mi, name, NULL, icon);
    systray_menu_add_item(mi->menu, "none", NULL, FALSE);
}

GtkWidget* systray_menu_add_submenu(GtkMenuShell* menu, menu_info_t* mi, const char* name, const char* tooltip, const char* icon)
{
    GtkWidget* submenu = gtk_menu_new();
    mi->menu = GTK_MENU_SHELL(submenu);
    mi->group = NULL;

    /* if this is the first item, remove the "none" placeholder label */
    if(mi->parent)
    {
        menu_info_t* parent_mi = mi->parent->menu_info;
        if(g_hash_table_size(parent_mi->items) == 0)
        {
            GList* children = gtk_container_get_children(GTK_CONTAINER(parent_mi->menu));
            if(children)
                gtk_container_remove(GTK_CONTAINER(parent_mi->menu), GTK_WIDGET(children->data));
        }
    }

    GtkWidget* item = systray_add_menu_item(menu, name, tooltip, icon);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);

    return item;
}

GtkWidget* systray_add_menu_item(GtkMenuShell* menu, const char* desc, const char* tooltip, const char* icon)
{
    GtkWidget* item = gtk_image_menu_item_new_with_mnemonic(desc);
    gtk_menu_shell_append(menu, item);

    if(tooltip)
        systray_set_tooltip(item, tooltip);

    if(icon)
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
            gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_MENU));

    gtk_widget_show(item);

    return item;
}

void systray_remove_menu_item(menu_info_t* mi, GtkWidget* item)
{
    gtk_container_remove(GTK_CONTAINER(mi->menu), item);

    /* insert "none" placeholder label for empty menus */
    if(!mi->parent)
        if(g_hash_table_size(mi->items) <= 1)
            systray_menu_add_item(mi->menu, "none", NULL, FALSE);
}

GtkWidget* systray_add_radio_item(menu_info_t* mi, const char* desc, const char* tooltip)
{
    GtkWidget* item = gtk_radio_menu_item_new_with_label(mi->group, desc);

    if(tooltip)
        systray_set_tooltip(item, tooltip);

    mi->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));

    /* if this is the first item, remove the "none" placeholder label */
    if(g_hash_table_size(mi->items) == 0)
    {
        GList* children = gtk_container_get_children(GTK_CONTAINER(mi->menu));
        if(children)
            gtk_container_remove(GTK_CONTAINER(mi->menu), GTK_WIDGET(children->data));
    }

    gtk_menu_shell_append(mi->menu, item);
    gtk_widget_show(item);

    return item;
}

void systray_remove_radio_item(menu_info_t* mi, GtkWidget* item)
{
    gtk_radio_menu_item_set_group(GTK_RADIO_MENU_ITEM(item), NULL);
    gtk_container_remove(GTK_CONTAINER(mi->menu), item);

    /* update group */
    GList* children = gtk_container_get_children(GTK_CONTAINER(mi->menu));
    if(children)
        mi->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(children->data));
    else
        mi->group = NULL;
}

GtkWidget* systray_menu_add_item(GtkMenuShell* menu, const char* name, const char* icon, gboolean sensitive)
{
    GtkWidget* item = systray_add_menu_item(menu, name, NULL, icon);
    gtk_widget_set_sensitive(item, sensitive);
    return item;
}

void systray_menu_add_application(GtkMenuShell* menu, const char* name, const char* icon, const char* command)
{
    gchar** exe = g_strsplit_set(command, " ", 2);
    gchar* c = g_find_program_in_path(exe[0]);
    g_strfreev(exe);
    GtkWidget* item = systray_menu_add_item(menu, name, icon, (c != NULL));
    g_free(c);

    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(start_application_cb), (gpointer)command);
}

void systray_remove_item(menu_info_item_t* mii)
{
    menu_info_t* mi = mii->menu_info;
    menu_infos_t* mis = mi->menu_infos;

    switch(mi->type)
    {
        case MENU_SERVER:
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_SINK:
            if(!mii->menu_info->parent)
                systray_remove_item_from_all_submenus(mii,
                        &mis->menu_info[MENU_INPUT]);
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_SOURCE:
            if(!mii->menu_info->parent)
                systray_remove_item_from_all_submenus(mii,
                        &mis->menu_info[MENU_OUTPUT]);
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            systray_remove_all_items_from_submenu(mii->submenu);
            systray_remove_menu_item(mi, mii->widget);
            break;
    }
}

void systray_add_all_items_to_submenu(menu_info_t* submenu, menu_info_item_t* item)
{
    GHashTableIter iter;
    gpointer key;
    menu_info_item_t* mii;

    g_hash_table_iter_init(&iter, submenu->items);

    while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        menu_info_subitem_add(item->submenu, mii->index, mii->name, mii->desc, NULL, NULL);
}

void systray_remove_all_items_from_submenu(menu_info_t* submenu)
{
    GHashTableIter iter;
    gpointer key;
    menu_info_item_t* mii;

    g_hash_table_iter_init(&iter, submenu->items);

    while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        systray_remove_menu_item(submenu, mii->widget);
}

void systray_add_item_to_all_submenus(menu_info_item_t* item, menu_info_t* submenu)
{
    GHashTableIter iter;
    gpointer key;
    menu_info_item_t* mii;

    g_hash_table_iter_init(&iter, submenu->items);

    while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        menu_info_subitem_add(mii->submenu, item->index, item->name, item->desc, NULL, NULL);
}

void systray_remove_item_from_all_submenus(menu_info_item_t* item, menu_info_t* submenu)
{
    GHashTableIter iter;
    gpointer key;
    menu_info_item_t* mii;

    g_hash_table_iter_init(&iter, submenu->items);

    while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        menu_info_item_remove(mii->submenu, item->index);
}

GtkWidget* systray_menu_item_about()
{
    GtkWidget* item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
    g_signal_connect(item, "activate", G_CALLBACK(systray_about_dialog), NULL);
    gtk_widget_show(item);
    return item;
}

void systray_about_dialog()
{
    GtkAboutDialog* dialog = ui_aboutdialog();
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_hide), dialog);
    gtk_widget_show_all(GTK_WIDGET(dialog));
}

GtkWidget* systray_menu_item_quit()
{
    GtkWidget* item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    g_signal_connect(item, "activate", G_CALLBACK(quit), NULL);
    gtk_widget_show(item);
    return item;
}

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer userdata)
{
    menu_infos_t* mis = userdata;

    switch(ev->button)
    {
        case 1:
        case 3:
            gtk_menu_popup(GTK_MENU(mis->menu), NULL, NULL, gtk_status_icon_position_menu, icon, ev->button, ev->time);
            break;
        case 2:
            {
                menu_info_t* mi = &mis->menu_info[(ev->state & GDK_CONTROL_MASK) ? MENU_SOURCE : MENU_SINK];
                menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);
                if(mii)
                    pulseaudio_toggle_mute(mii);
            }
            break;
    }
}

void systray_scroll_cb(systray_t* systray, guint state, GdkScrollDirection direction, menu_infos_t* mis)
{
    int inc = 0;

    switch(direction)
    {
        case GDK_SCROLL_UP:
            inc = 1;
            break;
        case GDK_SCROLL_DOWN:
            inc = -1;
            break;
        default:
            return;
    }

    menu_info_t* mi = &mis->menu_info[(state & GDK_CONTROL_MASK) ? MENU_SOURCE : MENU_SINK];
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);

    if(mii)
        pulseaudio_volume(mii, inc);
}

void start_application_cb(GtkMenuItem* item, const char* command)
{
    g_spawn_command_line_async(command, NULL);
}

void systray_set_tooltip(GtkWidget* item, const char* tooltip)
{
    char* escaped = g_markup_escape_text(tooltip, -1);
    char* markup = g_strdup_printf("<span font_family=\"monospace\" font_size=\"x-small\">%s</span>", escaped);
    gtk_widget_set_tooltip_markup(item, markup);
    g_free(escaped);
    g_free(markup);
}
