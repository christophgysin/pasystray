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
#include "config.h"
#include "options.h"

static gboolean version = FALSE;
static gboolean debug = FALSE;
static int volume_max = 0;
static int volume_inc = 1;
static gboolean no_notify = FALSE;
static gboolean always_notify = FALSE;
static gboolean monitors = FALSE;
// static gchar notify_mode[10][20];
static gchar **notify_mode;

static GOptionEntry entries[] =
{
    { "version", 'V', 0, G_OPTION_ARG_NONE, &version, "print version and exit", NULL },
    { "debug", 'd', 0, G_OPTION_ARG_NONE, &debug, "print debugging information", NULL },
    { "max-volume", 0, 0, G_OPTION_ARG_INT, &volume_max, "deprecated, use volume-max instead", "N" },
    { "volume-max", 'm', 0, G_OPTION_ARG_INT, &volume_max, "maximum volume (in percent)", "N" },
    { "volume-inc", 'i', 0, G_OPTION_ARG_INT, &volume_inc, "volume increment", "N" },
    { "no-notify", 'n', 0, G_OPTION_ARG_NONE, &no_notify, "disable all notifications", NULL },
    { "always-notify", 'a', 0, G_OPTION_ARG_NONE, &always_notify,
        "enable notifications for all changes in pulsaudio", NULL },
    { "include-monitors", 'n', 0, G_OPTION_ARG_NONE, &monitors, "include monitor sources", NULL },
    { "notify", 'N', 0, G_OPTION_ARG_STRING_ARRAY, &notify_mode, "notify mode", "NOTIFICATION TYPE" },
    { .long_name = NULL }
};

GOptionEntry* get_options()
{
    return entries;
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

    settings->volume_inc = 1;
    if(volume_inc > 0)
    {
        settings->volume_inc = volume_inc;
    }

    settings->notify = NOTIFY_DEFAULT;
    if(no_notify)
    {
        settings->notify = NOTIFY_NEVER;
    }
    if(always_notify)
    {
        settings->notify = NOTIFY_ALWAYS;
    }

    if(notify_mode)
    {
        /*  PLANNED MODES
            all             notify for all detected changes
            none            notify none
            new             notify when new sinks/sources are detected
            sink_all        notify for changes to all sinks
            sink_default    notify for changes to the default sink
            source_all
            source_default  notify for the default source
            stream_all      notify for all streams   
            stream_output   notify for output (playback) streams
            stream_input    notify for input (recording) streams

            help            SPECIAL: List possible modes and exit
        */
        for (int i = 0; notify_mode[i]; i++) {
            if(!g_strcmp0(notify_mode[i], "all"))
            {
                // TODO: Implement
            }
            else if(!g_strcmp0(notify_mode[i], "none"))
            {
                // TODO: Implement
            }
            else if(!g_strcmp0(notify_mode[i], "sink_all"))
            {
                settings->n_sink_all = TRUE;
            }
            else if(!g_strcmp0(notify_mode[i], "sink_default"))
            {
                settings->n_sink_default = TRUE; 
            }

        }
    }

    settings->monitors = monitors;
}
