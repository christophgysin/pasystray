#include <stdlib.h>
#include "systray.h"

static const char* COMMAND_PAMAN = "paman";
static const char* COMMAND_PAVUCONTROL = "pavucontrol";
static const char* COMMAND_PAVUMETER = "pavumeter";
static const char* COMMAND_PAVUMETER_REC = "pavumeter --record";
static const char* COMMAND_PAPREFS = "paprefs";

userdata_t* u;

void systray_create()
{
    u = malloc(sizeof(userdata_t));
    u->sinks = malloc(sizeof(menu_info_t));
    u->sources = malloc(sizeof(menu_info_t));
    u->servers = malloc(sizeof(menu_info_t));

    systray_create_gui();
}

void systray_create_gui()
{
    u->icon = gtk_status_icon_new_from_file("pulseaudio.png");
    systray_menu_create();
    g_signal_connect(u->icon, "button-press-event", G_CALLBACK(systray_click_cb), u);
    gtk_status_icon_set_tooltip_text(u->icon, "connecting to server...");
    gtk_status_icon_set_visible(u->icon, TRUE);
}

void systray_menu_create()
{
    u->menu = gtk_menu_new();

    systray_menu_add_submenu(u->menu, "Servers", u->servers, "network-wired");
    systray_menu_add_submenu(u->menu, "Sinks", u->sinks, "audio-card");
    systray_menu_add_submenu(u->menu, "Sources", u->sources, "audio-input-microphone");
    systray_menu_add_separator();

    systray_menu_add_application("_Manager...", NULL, COMMAND_PAMAN);
    systray_menu_add_application("_Volume Control...", NULL, COMMAND_PAVUCONTROL);
    systray_menu_add_application("_Volume Meter (Playback)...", NULL, COMMAND_PAVUMETER);
    systray_menu_add_application("_Volume Meter (Recording)...", NULL, COMMAND_PAVUMETER_REC);
    systray_menu_add_application("_Configure Local Sound Server...", NULL, COMMAND_PAPREFS);

    /*
    gtk_menu_shell_append(GTK_MENU_SHELL(u->menu), gtk_separator_menu_item_new());
    item = append_menuitem(u->menu, "_Preferences...", "gtk-preferences");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(show_preferences), NULL);
    */

    systray_menu_add_separator();
    gtk_menu_shell_append(GTK_MENU_SHELL(u->menu), systray_menu_item_quit());
}

void systray_menu_add_separator()
{
    GtkWidget* item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(u->menu), item);
    gtk_widget_show(item);
}

void systray_menu_add_submenu(GtkWidget* menu, const char* text, menu_info_t* m, const char* icon)
{
    GtkWidget* submenu = gtk_menu_new();
    m->menu = GTK_MENU_SHELL(submenu);
    m->group = NULL;

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

void systray_submenu_add_radio_item(menu_info_t* m, const char* name)
{
    GtkWidget* item = gtk_radio_menu_item_new_with_label(m->group, name);
    if(!m->group)
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);

    m->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
    gtk_menu_shell_append(m->menu, item);
    gtk_widget_show(item);
}

void systray_menu_add_application(const char* text, const char* icon, const char* command)
{
    GtkWidget* item = systray_create_menuitem(GTK_MENU(u->menu), text, icon);

    gchar** exe = g_strsplit_set(command, " ", 2);
    gchar* c = g_find_program_in_path(exe[0]);
    gtk_widget_set_sensitive(item, (c != NULL));
    g_free(c);
    g_strfreev(exe);

    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(start_application_cb), (gpointer)command);
}

void start_application_cb(GtkMenuItem* menuitem, const char* command)
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
    userdata_t* u = userdata;
    gtk_menu_popup(GTK_MENU(u->menu), NULL, NULL, gtk_status_icon_position_menu, u->icon, ev->button, ev->time);
}
