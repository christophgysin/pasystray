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

#include "systray_impl.h"

#include "config.h"
#include "systray.h"

#if defined(HAVE_APPINDICATOR) || defined(HAVE_AYATANA_APPINDICATOR)

#ifdef HAVE_AYATANA_APPINDICATOR
#include <libayatana-appindicator/app-indicator.h>
#else
#include <libappindicator/app-indicator.h>
#endif

static void systray_impl_scroll_cb(AppIndicator* appind, gint delta, GdkScrollDirection direction, gpointer userdata)
{
    guint state = 0; // TODO: get modifiers?
    menu_infos_t* mis = userdata;
    systray_scroll_cb(state, direction, mis);
}

systray_t systray_impl_create(menu_infos_t* mis)
{
    g_debug("[systray_impl] using AppIndicator");

    AppIndicator* appind = app_indicator_new("pasystray", "pasystray", APP_INDICATOR_CATEGORY_HARDWARE);
    app_indicator_set_status(appind, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_icon_full(appind, "pasystray", "pasystray");
    app_indicator_set_menu(appind, GTK_MENU(mis->menu));
    g_signal_connect(appind, "scroll-event", G_CALLBACK(systray_impl_scroll_cb), mis);
    return appind;
}

void systray_impl_set_icon(systray_t systray, const char* icon_name)
{
    AppIndicator* appind = systray;
    app_indicator_set_icon_full(appind, icon_name, icon_name);
}

void systray_impl_set_tooltip(systray_t systray, const char* markup)
{
    // TODO: set tooltip?
}

void systray_impl_set_has_tooltip(systray_t systray, gboolean has_tooltip)
{
    // TODO: enable/disable tooltip?
}
#elif HAVE_STATUSICON

#include "ui.h"

static void systray_impl_scroll_cb(GtkStatusIcon* icon, GdkEventScroll* ev, gpointer userdata)
{
    menu_infos_t* mis = userdata;
    systray_scroll_cb(ev->state, ev->direction, mis);
}

static GtkStatusIcon* systray_impl_create_statusicon()
{
    GtkStatusIcon* status_icon = gtk_status_icon_new();
    gtk_status_icon_set_from_icon_name(status_icon, "pasystray");
    gtk_status_icon_set_title(status_icon, "PulseAudio system tray");

    GError* error = NULL;
    gchar* exe_path = g_file_read_link("/proc/self/exe", &error);
    if(error)
    {
        g_debug("failed to read /proc/self/exe: %s", error->message);
        g_error_free(error);
        g_free(exe_path);
        return status_icon;
    }

    gchar* dirname = g_path_get_dirname(exe_path);
    g_free(exe_path);

    gchar* icon_file = g_build_path("/", dirname, "../data/pasystray.svg", NULL);
    g_free(dirname);

    if(g_file_test(icon_file, G_FILE_TEST_EXISTS))
    {
        gtk_status_icon_set_from_file(status_icon, icon_file);
        g_debug("using icon: %s", icon_file);
    }
    g_free(icon_file);

    return status_icon;
}

systray_t systray_impl_create(menu_infos_t* mis)
{
    g_debug("[systray_impl] using GtkStatusIcon");

    GtkStatusIcon* icon = systray_impl_create_statusicon();
    g_signal_connect(icon, "button-press-event", G_CALLBACK(systray_click_cb), mis);
    g_signal_connect(icon, "scroll-event", G_CALLBACK(systray_impl_scroll_cb), mis);
    gtk_status_icon_set_tooltip_text(icon, "connecting to server...");
    gtk_status_icon_set_visible(icon, TRUE);

    return icon;
}

void systray_impl_print_icon(gchar* icon_name, gpointer userdata)
{
    if(icon_name)
        g_debug(" %s", icon_name);
}

void systray_impl_list_icons(GtkIconTheme* icon_theme)
{
    GList* icons = gtk_icon_theme_list_icons(icon_theme, NULL);
    GList* sorted_icons = g_list_sort(icons, (GCompareFunc)g_ascii_strcasecmp);
    g_list_foreach(icons, (GFunc)g_free, NULL);
    g_list_free(icons);

    g_list_foreach(sorted_icons, (GFunc)systray_impl_print_icon, NULL);
    g_list_foreach(sorted_icons, (GFunc)g_free, NULL);
    g_list_free(sorted_icons);
}

void systray_impl_set_icon(systray_t systray, const char* icon_name)
{
    GtkStatusIcon* icon = systray;

    GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
    if(gtk_icon_theme_has_icon(icon_theme, icon_name) == FALSE)
    {
        g_warning("[systray_impl] can't find icon %s in current icon theme", icon_name);
        g_debug("[systray_impl] available icons in current theme:");
        systray_impl_list_icons(icon_theme);
    }

    gtk_status_icon_set_from_icon_name(icon, icon_name);
}

void systray_impl_set_tooltip(systray_t systray, const char* markup)
{
    GtkStatusIcon* icon = systray;
    gtk_status_icon_set_tooltip_markup(icon, markup);
}

void systray_impl_set_has_tooltip(systray_t systray, gboolean has_tooltip)
{
    GtkStatusIcon* icon = systray;
    gtk_status_icon_set_has_tooltip(icon, has_tooltip);
}

#else

static void systray_impl_scroll_cb(GtkImage* image, GdkEventScroll* ev, gpointer userdata)
{
    menu_infos_t* mis = userdata;
    systray_scroll_cb(ev->state, ev->direction, mis);
}

systray_t systray_impl_create(menu_infos_t* mis)
{
    g_debug("[systray_impl] using GtkWindow");

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "pasystray");

    GtkWidget *button = gtk_menu_button_new();
    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(button), GTK_WIDGET(mis->menu));
    gtk_widget_add_events(button, GDK_SCROLL_MASK);
    g_signal_connect(button, "button-press-event", G_CALLBACK(systray_click_cb), mis);
    g_signal_connect(button, "scroll-event", G_CALLBACK(systray_impl_scroll_cb), mis);

    GtkWidget *image = gtk_image_new();
    gtk_image_set_from_icon_name(GTK_IMAGE(image), "pasystray", GTK_ICON_SIZE_DIALOG);
    gtk_button_set_image(GTK_BUTTON(button), image);

    gtk_widget_show_all(window);

    return image;
}

void systray_impl_set_icon(systray_t systray, const char* icon_name)
{
    GtkImage *image = systray;
    gtk_image_set_from_icon_name(image, icon_name, GTK_ICON_SIZE_DIALOG);
}

void systray_impl_set_tooltip(systray_t systray, const char* markup)
{
    GtkImage *image = systray;
    gtk_widget_set_tooltip_markup(GTK_WIDGET(image), markup);
}

void systray_impl_set_has_tooltip(systray_t systray, gboolean has_tooltip)
{
    GtkImage *image = systray;
    gtk_widget_set_has_tooltip(GTK_WIDGET(image), has_tooltip);
}

#endif
