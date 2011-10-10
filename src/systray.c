#include "systray.h"

void systray_create(menu_infos_t* mis)
{
    GtkStatusIcon* icon = gtk_status_icon_new_from_file("pulseaudio.png");
    systray_menu_create(mis);
    g_signal_connect(icon, "button-press-event", G_CALLBACK(systray_click_cb), mis);
    gtk_status_icon_set_tooltip_text(icon, "connecting to server...");
    gtk_status_icon_set_visible(icon, TRUE);
}

void systray_menu_create(menu_infos_t* mis)
{
    mis->menu = GTK_MENU_SHELL(gtk_menu_new());

    systray_menu_add_submenu(mis->menu, "Default Server", mis->servers, "network-wired");
    systray_menu_add_submenu(mis->menu, "Default Sink", mis->sinks, "audio-card");
    systray_menu_add_submenu(mis->menu, "Default Source", mis->sources, "audio-input-microphone");
    systray_menu_add_submenu(mis->menu, "Playback Streams", mis->sink_inputs, "player_play");
    systray_menu_add_submenu(mis->menu, "Recording Streams", mis->source_outputs, "player_record");
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

void systray_menu_add_submenu(GtkMenuShell* menu, const char* text, menu_info_t* mi, const char* icon)
{
    GtkWidget* submenu = gtk_menu_new();
    mi->menu = GTK_MENU_SHELL(submenu);
    mi->group = NULL;

    GtkWidget* item = systray_create_menuitem(GTK_MENU(menu), text, icon);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
}

GtkWidget* systray_create_menuitem(GtkMenu* m, const char* text, const char* icon)
{
    GtkWidget* item = gtk_image_menu_item_new_with_mnemonic(text);
    gtk_menu_shell_append(GTK_MENU_SHELL(m), item);

    if(icon)
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
            gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_MENU));

    gtk_widget_show(item);

    return item;
}

GtkWidget* systray_submenu_add_radio_item(menu_info_t* mi, const char* text)
{
    GtkWidget* item = gtk_radio_menu_item_new_with_label(mi->group, text);
    if(!mi->group)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);

    mi->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
    gtk_menu_shell_append(mi->menu, item);
    gtk_widget_show(item);

    return item;
}

GtkWidget* systray_submenu_add_menu_item(menu_info_t* mi, const char* text, const char* icon)
{
    return systray_create_menuitem(GTK_MENU(mi->menu), text, icon);
}

void systray_menu_add_application(GtkMenuShell* menu, const char* text, const char* icon, const char* command)
{
    GtkWidget* item = systray_create_menuitem(GTK_MENU(menu), text, icon);

    gchar** exe = g_strsplit_set(command, " ", 2);
    gchar* c = g_find_program_in_path(exe[0]);
    gtk_widget_set_sensitive(item, (c != NULL));
    g_free(c);
    g_strfreev(exe);

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
