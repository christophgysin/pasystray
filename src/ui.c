
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

#include <glib.h>
#include "menu_info.h"
#include "ui.h"
#include "config.h"
#include "systray_impl.h"

typedef enum {
    ICON_IDX_MUTED = 0,
    ICON_IDX_ZERO = 1,
    ICON_IDX_LOW = 2,
    ICON_IDX_MEDIUM = 3,
    ICON_IDX_HIGH = 4
} icon_idx_t;

enum { ICON_IDX_COUNT = 5 };

typedef const gchar* icon_set_t[ICON_IDX_COUNT];

static icon_set_t volume_icon_names = {
    [ICON_IDX_MUTED] = "audio-volume-muted",
    [ICON_IDX_ZERO] = "audio-volume-zero",
    [ICON_IDX_LOW] = "audio-volume-low",
    [ICON_IDX_MEDIUM] = "audio-volume-medium",
    [ICON_IDX_HIGH] = "audio-volume-high"
};

static icon_set_t mic_icon_names = {
    [ICON_IDX_MUTED] = "microphone-sensitivity-muted",
    [ICON_IDX_ZERO] = "microphone-sensitivity-zero",
    [ICON_IDX_LOW] = "microphone-sensitivity-low",
    [ICON_IDX_MEDIUM] = "microphone-sensitivity-medium",
    [ICON_IDX_HIGH] = "microphone-sensitivity-high"
};

static icon_set_t ui_icon_names[MENU_COUNT];

static GtkBuilder* builder;

static void ui_load_icons(void);

void ui_load()
{
    builder = gtk_builder_new();

    GError* error = NULL;

    // GLADE_FILE is set in src/Makefile.am
    const char* filename = GLADE_FILE;

    /* try to load ui in current dir first */
    gchar* local_file = g_strdup_printf("pasystray.gtk%u.glade", GTK_VERSION_MAJOR);
    if(g_file_test(local_file, G_FILE_TEST_EXISTS))
        filename = local_file;
    gchar* local_file_src = g_strdup_printf("src/pasystray.gtk%u.glade", GTK_VERSION_MAJOR);
    if(g_file_test(local_file_src, G_FILE_TEST_EXISTS))
        filename = local_file_src;

    g_debug("using UI file: %s", filename);

    guint ret = gtk_builder_add_from_file(builder, filename, &error);

    g_free(local_file);
    g_free(local_file_src);

    ui_load_icons();

    if(!ret)
    {
        g_error("[ui] %s", error->message);
    }
}

void ui_set_volume_icon(menu_info_item_t* mii)
{
    g_debug("pulseaudio_set_volume_icon(%s)", mii->name);

    pa_volume_t volume = pa_cvolume_max(mii->volume);

    g_debug("volume:%u%s", volume, mii->mute ? " muted" : "");

    const char* icon_name = NULL;
    icon_idx_t idx;

    if(mii->mute)
        idx = ICON_IDX_MUTED;
    else if(volume == 0)
        idx = ICON_IDX_ZERO;
    else if(volume < (PA_VOLUME_NORM / 3))
        idx = ICON_IDX_LOW;
    else if(volume < (PA_VOLUME_NORM / 3 * 2))
        idx = ICON_IDX_MEDIUM;
    else
        idx = ICON_IDX_HIGH;

    icon_name = ui_icon_names[mii->menu_info->type][idx];
    g_free(mii->icon);
    mii->icon = g_strdup(icon_name);
}

void ui_update_systray_icon(menu_info_item_t* mii)
{
    g_debug("pulseaudio_update_systray_icon(%s)", mii->name);

    ui_set_volume_icon(mii);

    menu_infos_t* mis = mii->menu_info->menu_infos;
    systray_impl_set_icon(mis->systray, mii->icon);
}

GtkDialog* ui_renamedialog()
{
    return (GtkDialog*) gtk_builder_get_object(builder, "renamedialog");
}

GtkLabel* ui_renamedialog_label()
{
    return (GtkLabel*) gtk_builder_get_object(builder, "label");
}

GtkEntry* ui_renamedialog_entry()
{
    return (GtkEntry*) gtk_builder_get_object(builder, "entry");
}

GtkDialog* ui_errordialog(const gchar* title, const gchar* message)
{
    GtkMessageDialog* dialog = (GtkMessageDialog*) gtk_builder_get_object(builder, "errordialog");

    gtk_message_dialog_set_markup(dialog, title);

    if(message)
        gtk_message_dialog_format_secondary_text(dialog, "%s", message);

    return GTK_DIALOG(dialog);
}

static const gchar* ui_find_icon_name(GtkIconTheme* theme, const gchar* name, const gchar* fallback)
{
    const gchar* s = g_strdup_printf("%s-symbolic", name);
    if(gtk_icon_theme_has_icon(theme, s))
        return s;
    g_free(s);
    if(gtk_icon_theme_has_icon(theme, name))
        return g_strdup(name);
    return g_strdup(fallback);
}

static void ui_load_icons(void) {
    GtkIconTheme* theme = gtk_icon_theme_get_default();
    for(menu_type_t mt = 0; mt < MENU_COUNT; mt++) {
        const gchar** preferred_names = (mt == MENU_SOURCE ? mic_icon_names : volume_icon_names);
        for(icon_idx_t i = 0; i < ICON_IDX_COUNT; i++) {
            ui_icon_names[mt][i] = ui_find_icon_name(theme, preferred_names[i], volume_icon_names[i]);
        }
    }
}
