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

GMainLoop* loop;
menu_infos_t* mis;

int main(int argc, char *argv[])
{
    parse_options(argc, argv);
    gtk_init(&argc, &argv);

    init();
    g_main_loop_run(loop);
    destroy();

    return 0;
}

void init()
{
    loop = g_main_loop_new(NULL, FALSE);

    avahi_init(loop);
    notify_initialize();
    x11_property_init();

    ui_load();

    mis = menu_infos_create();
    systray_create(mis);
    menu_infos_init(mis);

    pulseaudio_init(mis);
    avahi_start(mis);
}

void quit()
{
    g_main_loop_quit(loop);
}

void destroy()
{
    pulseaudio_destroy();
    avahi_destroy();
    menu_infos_destroy(mis);
}
