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

#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif
#include "config.h"
#include "options.h"

static gboolean version = FALSE;
static gboolean debug = FALSE;
static int volume_max = 0;
static int volume_inc = 2;
static gboolean reverse_scroll = FALSE;
static gboolean icon_tooltip = TRUE;
static gboolean no_notify = FALSE;
static gboolean always_notify = FALSE;
static gboolean monitors = FALSE;
static gboolean key_grabbing = FALSE;
static gchar **notify_mode;
static gboolean symbolic_icons = FALSE;

static GOptionEntry entries[] =
{
    { "version", 'V', 0, G_OPTION_ARG_NONE, &version, "Print version and exit", NULL },
    { "debug", 'd', 0, G_OPTION_ARG_NONE, &debug, "Print debugging information", NULL },
    { "volume-max", 'm', 0, G_OPTION_ARG_INT, &volume_max, "Maximum volume (in percent)", "N" },
    { "volume-inc", 'i', 0, G_OPTION_ARG_INT, &volume_inc, "Volume increment", "N" },
    { "reverse-scroll", 'r', 0, G_OPTION_ARG_NONE,
        &reverse_scroll, "Reverse volume scroll direction", NULL },
    { "no-icon-tooltip", 't', 0, G_OPTION_ARG_NONE,
        &icon_tooltip, "Disable the status icon tooltip for the connected state", NULL },
    { "no-notify", 'n', 0, G_OPTION_ARG_NONE, &no_notify,
        "Deprecated, use --notify=none instead", NULL },
    { "always-notify", 'a', 0, G_OPTION_ARG_NONE, &always_notify,
        "Deprecated, use --notify=all instead", NULL },
    { "include-monitors", 'n', 0, G_OPTION_ARG_NONE, &monitors, "Include monitor sources", NULL },
    { "key-grabbing",  'g', 0, G_OPTION_ARG_NONE, &key_grabbing, "Grab volume control keys", NULL },
    { "notify", 'N', 0, G_OPTION_ARG_STRING_ARRAY, &notify_mode,
        "Set notification options, use --notify=help for a list of valid options", "OPTION" },
    { "symbolic-icons", 'S', 0, G_OPTION_ARG_NONE, &symbolic_icons,
        "Prefer symbolic icons", NULL },
    { .long_name = NULL }
};

GOptionEntry* get_options()
{
    return entries;
}

// Set some default values close to previous behavior
void notify_default(settings_t* settings)
{
    settings->n_new = TRUE;
    settings->n_sink = FALSE;
    settings->n_sink_default = FALSE;
    settings->n_source = FALSE;
    settings->n_source_default = FALSE;
    settings->n_stream_output = FALSE;
    settings->n_stream_input = FALSE;
    settings->n_systray_action = TRUE;
}

void notify_all(settings_t* settings)
{
    settings->n_new = TRUE;
    settings->n_sink = TRUE;
    settings->n_sink_default = TRUE;
    settings->n_source = TRUE;
    settings->n_source_default = TRUE;
    settings->n_stream_output = TRUE;
    settings->n_stream_input = TRUE;
    settings->n_systray_action = TRUE;
}

void notify_none(settings_t* settings)
{
    settings->n_new = FALSE;
    settings->n_sink = FALSE;
    settings->n_sink_default = FALSE;
    settings->n_source = FALSE;
    settings->n_source_default = FALSE;
    settings->n_stream_output = FALSE;
    settings->n_stream_input = FALSE;
    settings->n_systray_action = FALSE;
}

void parse_options(settings_t* settings)
{
    if(version)
    {
        g_print("%s\n", VERSION);
        exit(0);
    }

    if(debug)
    {
        setenv("G_MESSAGES_DEBUG", "pasystray", 1);
    }

    settings->volume_max = 0;
    if(volume_max > 0)
    {
        settings->volume_max = volume_max;
    }

    settings->volume_inc = 2;
    if(volume_inc > 0)
    {
        settings->volume_inc = volume_inc;
    }

    settings->reverse_scroll = reverse_scroll;
    settings->icon_tooltip = icon_tooltip;

    notify_default(settings);

    if(notify_mode)
    {
        for (int i = 0; notify_mode[i]; i++) {
            if(!g_strcmp0(notify_mode[i], "all"))
            {
                notify_all(settings);
            }
            else if(g_str_equal(notify_mode[i], "none"))
            {
                notify_none(settings);
            }
            else if(g_str_equal(notify_mode[i], "new"))
            {
                settings->n_new = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "sink"))
            {
                settings->n_sink = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "sink_default"))
            {
                settings->n_sink_default = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "source"))
            {
                settings->n_source = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "source_default"))
            {
                settings->n_source_default = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "stream"))
            {
                settings->n_stream_output = TRUE;
                settings->n_stream_input = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "stream_output"))
            {
                settings->n_stream_output = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "stream_input"))
            {
                settings->n_stream_input = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "systray_action"))
            {
                settings->n_systray_action = TRUE;
            }
            else if(g_str_equal(notify_mode[i], "help"))
            {
                gchar *help_text=(
                    "Notification options:\n"
                    "  all                     Notify for all detected changes\n"
                    "  none                    Never notify, except for options set after this one\n"
                    "  new                     Notify when new sinks/sources are added\n"
                    "  sink                    Notify for changes to all sinks\n"
                    "  sink_default            Notify for changes to the default sink\n"
                    "  source                  Notify for changes to all sources\n"
                    "  source_default          Notify for changes to the default source\n"
                    "  stream                  Notify for all streams\n"
                    "  stream_output           Notify for output (playback) streams\n"
                    "  stream_input            Notify for input (recording) streams\n"
                    "  systray_action          Notify for changes made through pasystray\n"
                    "  help                    List possible options and exit\n"
                );

                g_print("%s",help_text);
                exit(0);
            }
            else
            {
                g_print("Warning: Invalid notification option \"%s\". Run 'pasystray --notify=help' for a list of valid options.\n", notify_mode[i]);
            }
        }
    }

    if(no_notify)
    {
        g_warning("--no-notify is deprecated, use --notify=none instead");
        notify_none(settings);
    }
    if(always_notify)
    {
        g_warning("--always-notify is deprecated, use --notify=all instead");
        notify_all(settings);
    }

    settings->monitors = monitors;
    settings->key_grabbing = key_grabbing;
#ifndef GDK_WINDOWING_X11
    settings->key_grabbing = FALSE;
#elif GTK_CHECK_VERSION(3,0,0)
    if (!GDK_IS_X11_DISPLAY(gdk_display_get_default())) {
        settings->key_grabbing = FALSE;
    }
#endif
    if (key_grabbing && !settings->key_grabbing) {
        g_warning("--key-grabbing can only be used on X11, option ignored");
    }

    settings->symbolic_icons = symbolic_icons;
}
