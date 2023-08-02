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
#include <gtk/gtk.h>

#include "pasystray.h"
#include "options.h"
#include "notify.h"
#include "ui.h"
#include "menu_info.h"
#include "systray.h"
#include "pulseaudio.h"
#include "avahi.h"
#include "x11-property.h"
#include "x11-key-grabber.h"

static GMainLoop* loop;
static menu_infos_t* mis;

int main(int argc, char *argv[])
{
    GOptionEntry* options = get_options();
    GError *error = NULL;
    gtk_init_with_args(&argc, &argv, NULL, options, NULL, &error);
    if(error)
    {
        g_print("option parsing failed: %s\n", error->message);
        return EXIT_FAILURE;
    }

    settings_t settings;
    parse_options(&settings);

    init(&settings);
    g_main_loop_run(loop);
    destroy(&settings);

    return EXIT_SUCCESS;
}

void init(settings_t* settings)
{
    loop = g_main_loop_new(NULL, FALSE);

    avahi_init(loop);
    notify_initialize();
    x11_property_init();

    ui_load(settings);

    mis = menu_infos_create();
    mis->settings = *settings;
    systray_create(mis);
    menu_infos_init(mis);

    pulseaudio_init(mis);
    avahi_start(mis);

#ifdef HAVE_X11
    if (settings->key_grabbing)
      key_grabber_grab_keys(mis);
#endif
}

void quit()
{
    g_main_loop_quit(loop);
}

void destroy(settings_t* settings)
{
#ifdef HAVE_X11
    if (settings->key_grabbing)
      key_grabber_ungrab_keys(mis);
#endif

    pulseaudio_destroy();
    avahi_destroy();
    menu_infos_destroy(mis);
}
