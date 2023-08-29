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

#include "config.h"

#ifndef HAVE_X11

#include <stdlib.h>

void x11_property_init(){}
void x11_property_set(const char* key, const char* value){}
void x11_property_del(const char* key){}
char* x11_property_get(const char* key){ return NULL; }

#else

#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

static Display* display = NULL;
static Window window;

void x11_property_init()
{
#ifndef GDK_WINDOWING_X11
    return;
#elif GTK_CHECK_VERSION(3,0,0)
    if (!GDK_IS_X11_DISPLAY(gdk_display_get_default())) {
        return;
    }
#endif
    display = gdk_x11_get_default_xdisplay();
    window = RootWindow(display, 0);
}

void x11_property_set(const char* key, const char* value)
{
    if (!display) {
        g_warning("x11_property_set: call has no effect on non-X sessions");
        return;
    }
    g_debug("[x11-property] setting '%s' to '%s'", key, value);

    Atom atom = XInternAtom(display, key, False);
    XChangeProperty(display, window, atom, XA_STRING, 8, PropModeReplace,
            (const unsigned char*) value, strlen(value)+1);
}

void x11_property_del(const char* key)
{
    if (!display) {
        g_warning("x11_property_del: call has no effect on non-X sessions");
        return;
    }
    g_debug("[x11-property] deleting '%s'", key);

    Atom atom = XInternAtom(display, key, False);
    XDeleteProperty(display, window, atom);
}

char* x11_property_get(const char* key)
{
    if (!display) {
        g_warning("x11_property_get: call has no effect on non-X sessions");
        return NULL;
    }
    Atom property = XInternAtom(display, key, False);
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char* prop = NULL;
    char* value = NULL;

    if(XGetWindowProperty(
                display, window, property, 0, 64, False, XA_STRING,
                &actual_type, &actual_format, &nitems, &bytes_after, &prop
                ) != Success)
        goto finish;

    if(actual_type != XA_STRING)
        goto finish;

    value = g_memdup2(prop, nitems + 1);

    g_debug("[x11-property] got '%s' = '%s'", key, value);

finish:
    if(prop)
        XFree(prop);

    return value;
}

#endif
