#include "systray.h"

void systray_create(menu_infos_t* mis)
{
    mis->icon = gtk_status_icon_new_from_file("pulseaudio.png");
    systray_menu_create(mis);
    g_signal_connect(mis->icon, "button-press-event", G_CALLBACK(systray_click_cb), mis);
    gtk_status_icon_set_tooltip_text(mis->icon, "connecting to server...");
    gtk_status_icon_set_visible(mis->icon, TRUE);
}

void systray_menu_create(menu_infos_t* mis)
{
    mis->menu = GTK_MENU_SHELL(gtk_menu_new());

    systray_menu_add_submenu(mis, MENU_SERVER, "Default Server",    NULL, "network-wired");
    systray_menu_add_submenu(mis, MENU_SINK,   "Default Sink",      NULL, "audio-card");
    systray_menu_add_submenu(mis, MENU_SOURCE, "Default Source",    NULL, "audio-input-microphone");
    systray_menu_add_submenu(mis, MENU_INPUT,  "Playback Streams",  NULL, "player_play");
    systray_menu_add_submenu(mis, MENU_OUTPUT, "Recording Streams", NULL, "player_record");
    systray_menu_add_separator(mis->menu);

    static const char* COMMAND_PAMAN = "paman";
    static const char* COMMAND_PAVUCONTROL = "pavucontrol";
    static const char* COMMAND_PAVUMETER = "pavumeter";
    static const char* COMMAND_PAVUMETER_REC = "pavumeter --record";
    static const char* COMMAND_PAPREFS = "paprefs";

    systray_menu_add_application(mis->menu, "_Manager...", NULL, COMMAND_PAMAN);
    systray_menu_add_application(mis->menu, "_Volume Control...", NULL, COMMAND_PAVUCONTROL);
    systray_menu_add_application(mis->menu, "_Volume Meter (Playback)...", NULL, COMMAND_PAVUMETER);
    systray_menu_add_application(mis->menu, "_Volume Meter (Recording)...", NULL, COMMAND_PAVUMETER_REC);
    systray_menu_add_application(mis->menu, "_Configure Local Sound Server...", NULL, COMMAND_PAPREFS);

    /*
    systray_menu_add_separator(mis->menu);
    item = append_menuitem(mis->menu, "_Preferences...", "gtk-preferences");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(show_preferences), NULL);
    */

    systray_menu_add_separator(mis->menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(mis->menu), systray_menu_item_quit());
}

void systray_menu_add_separator(GtkMenuShell* menu)
{
    GtkWidget* item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(menu, item);
    gtk_widget_show(item);
}

void systray_menu_add_submenu(menu_infos_t* mis, menu_type_t type, const char* name, char* desc, const char* icon)
{
    GtkMenuShell* menu = mis->menu;
    menu_info_t* mi = &mis->menu_info[type];

    GtkWidget* submenu = gtk_menu_new();
    mi->menu = GTK_MENU_SHELL(submenu);
    mi->group = NULL;

    GtkWidget* item = systray_add_menu_item(menu, name, desc, icon);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
}

GtkWidget* systray_add_menu_item(GtkMenuShell* menu, const char* name, char* desc, const char* icon)
{
    GtkWidget* item = gtk_image_menu_item_new_with_mnemonic(name);
    gtk_menu_shell_append(menu, item);

    if(desc)
    {
        char* markup = g_strdup_printf("<span font_family=\"monospace\" font_size=\"20\">%s</span>", desc);
        gtk_widget_set_tooltip_markup(item, markup);
        g_free(markup);
    }

    if(icon)
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
            gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_MENU));

    gtk_widget_show(item);

    return item;
}

void systray_remove_menu_item(menu_info_t* mi, GtkWidget* item)
{
    gtk_container_remove(GTK_CONTAINER(mi->menu), item);
}

GtkWidget* systray_add_radio_item(menu_info_t* mi, const char* name, char* desc)
{
    GtkWidget* item = gtk_radio_menu_item_new_with_label(mi->group, name);

    if(!mi->group)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);

    if(desc)
    {
        char* markup = g_strdup_printf("<span font_family=\"monospace\" font_size=\"x-small\">%s</span>", desc);
        gtk_widget_set_tooltip_markup(item, markup);
        g_free(markup);
    }

    mi->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
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

void systray_menu_add_application(GtkMenuShell* menu, const char* name, const char* icon, const char* command)
{
    GtkWidget* item = systray_add_menu_item(menu, name, NULL, icon);

    gchar** exe = g_strsplit_set(command, " ", 2);
    gchar* c = g_find_program_in_path(exe[0]);
    g_strfreev(exe);
    gtk_widget_set_sensitive(item, (c != NULL));
    g_free(c);

    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(start_application_cb), (gpointer)command);
}

void start_application_cb(GtkMenuItem* item, const char* command)
{
    g_spawn_command_line_async(command, NULL);
}

GtkWidget* systray_menu_item_quit()
{
    GtkWidget* item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    g_signal_connect(item, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(item);
    return item;
}

void systray_click_cb(GtkStatusIcon* icon, GdkEventButton* ev, gpointer userdata)
{
    menu_infos_t* mis = userdata;
    gtk_menu_popup(GTK_MENU(mis->menu), NULL, NULL, gtk_status_icon_position_menu, icon, ev->button, ev->time);
}
