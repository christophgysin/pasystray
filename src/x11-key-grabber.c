/*
 * This file is part of pa-applet.
 *
 * © 2012 Fernando Tarlá Cardoso Lemos
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <glib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>

#include "x11-key-grabber.h"
#include "pulseaudio_action.h"

#define NUM_KEYS_TO_GRAB 4

void volume_raise_cb(unsigned int state, menu_infos_t *mis)
{
    menu_info_t* mi = &mis->menu_info[(state & ControlMask) ? MENU_SOURCE : MENU_SINK];
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);

    g_debug("[key_grabber] %sXF86AudioRaiseVolume pressed\n", (state & ControlMask) ? "Ctrl-" : "");
    if(mii)
        pulseaudio_volume(mii, mis->settings.volume_inc);
}

void volume_lower_cb(unsigned int state, menu_infos_t *mis)
{
    menu_info_t* mi = &mis->menu_info[(state & ControlMask) ? MENU_SOURCE : MENU_SINK];
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);

    g_debug("[key_grabber] %sXF86AudioLowerVolume pressed\n", (state & ControlMask) ? "Ctrl-" : "");
    if(mii)
         pulseaudio_volume(mii, -mis->settings.volume_inc);
}

void volume_mute_cb(unsigned int state, menu_infos_t *mis) {
    menu_info_t* mi = &mis->menu_info[(state & ControlMask) ? MENU_SOURCE : MENU_SINK];
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);

    g_debug("[key_grabber] %sXF86AudioMute pressed\n", (state & ControlMask) ? "Ctrl-" : "");
    if(mii)
        pulseaudio_toggle_mute(mii);
}

void mic_mute_cb(unsigned int state, menu_infos_t *mis) {
    menu_info_t* mi = &mis->menu_info[MENU_SOURCE];
    menu_info_item_t* mii = menu_info_item_get_by_name(mi, mi->default_name);

    g_debug("[key_grabber] XF86AudioMicMute pressed\n");
    if(mii)
        pulseaudio_toggle_mute(mii);
}

static key_grabber_cb *grabbers[NUM_KEYS_TO_GRAB] = {
    volume_raise_cb,
    volume_lower_cb,
    volume_mute_cb,
    mic_mute_cb
};

static const char *keysym_names[NUM_KEYS_TO_GRAB] = {
    "XF86AudioRaiseVolume",
    "XF86AudioLowerVolume",
    "XF86AudioMute",
    "XF86AudioMicMute"
};

static KeyCode grabbed_keys[NUM_KEYS_TO_GRAB] = { 0, };

static GdkFilterReturn filter_func(GdkXEvent *gdk_xevent, GdkEvent *event, gpointer data)
{
    // Skip events other than key presses
    XEvent *xevent = (XEvent *)gdk_xevent;
    if (xevent->type != KeyPress)
        return GDK_FILTER_CONTINUE;

    // Find a match for the key press
    XKeyEvent *keyevent = (XKeyEvent *)xevent;
    for (int i = 0; i < NUM_KEYS_TO_GRAB; ++i) {
        if (keyevent->keycode == grabbed_keys[i]) {
            if (grabbers[i] != NULL)
                (*grabbers[i])(keyevent->state, data);
            return GDK_FILTER_REMOVE;
        }
    }

    // Continue processing this event
    return GDK_FILTER_CONTINUE;
}

void key_grabber_grab_keys(menu_infos_t *mis)
{
    // Find the X11 display
    GdkDisplay *gdkDisplay = gdk_display_get_default();
    Display *dpy = GDK_DISPLAY_XDISPLAY(gdkDisplay);

    // Resolve the keysym names into keycodes
    for (int i = 0; i < NUM_KEYS_TO_GRAB; ++i) {
        // Resolve the keysym name into a keysym first
        KeySym keysym = XStringToKeysym(keysym_names[i]);
        if (keysym == NoSymbol) {
            g_printerr("Failed to resolve %s into a keysym\n", keysym_names[i]);
            continue;
        }

        // Resolve the keysym into a keycode
        grabbed_keys[i] = XKeysymToKeycode(dpy, keysym);
        if (grabbed_keys[i] == 0) {
            g_printerr("Failed to resolve %s into a keycode\n", keysym_names[i]);
            continue;
        }
    }

    int numScreens = 1;
#if GDK_VERSION_CUR_STABLE < G_ENCODE_VERSION(3, 10)
    numScreens = gdk_display_get_n_screens(gdkDisplay);
#endif

    // Grab the keys for all screens
    for (int i = 0; i < numScreens; ++i) {
#if GTK_CHECK_VERSION(3,20,0)
        GdkScreen *screen = gdk_display_get_default_screen(gdkDisplay);
#else
        GdkScreen *screen = gdk_display_get_screen(gdkDisplay, i);
#endif
        if (screen == NULL)
            continue;

        // Find the X11 root window
        GdkWindow *gdkRoot = gdk_screen_get_root_window(screen);
        Window root = GDK_WINDOW_XID(gdkRoot);

        for (int i = 0; i < NUM_KEYS_TO_GRAB; ++i) {
            // Ignore the keys that we couldn't resolve
            KeyCode keycode = grabbed_keys[i];
            if (keycode == 0)
                continue;

            // Try to grab the keycodes with any modifiers
#if GTK_CHECK_VERSION(3,22,0)
            gdk_x11_display_error_trap_push(gdkDisplay);
#else
            gdk_error_trap_push();
#endif
            XGrabKey(dpy, keycode, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);

#if GTK_CHECK_VERSION(3,22,0)
            gdk_display_flush(gdkDisplay);
#else
            gdk_flush();
#endif

            // Handle errors
#if GTK_CHECK_VERSION(3,22,0)
            if (gdk_x11_display_error_trap_pop(gdkDisplay))
#else
            if (gdk_error_trap_pop())
#endif
                g_printerr("Failed to grab %s\n", keysym_names[i]);
        }

        // Register for X events
        gdk_window_add_filter(gdkRoot, filter_func, mis);
    }
}

void key_grabber_ungrab_keys(menu_infos_t *mis)
{
    // Find the X11 display
    GdkDisplay *gdkDisplay = gdk_display_get_default();
    Display *dpy = GDK_DISPLAY_XDISPLAY(gdkDisplay);

    int numScreens = 1;
#if GDK_VERSION_CUR_STABLE < G_ENCODE_VERSION(3, 10)
    numScreens = gdk_display_get_n_screens(gdkDisplay);
#endif

    // Ungrab the keys for all screens
    for (int i = 0; i < numScreens; ++i) {
#if GTK_CHECK_VERSION(3,20,0)
        GdkScreen *screen = gdk_display_get_default_screen(gdkDisplay);
#else
        GdkScreen *screen = gdk_display_get_screen(gdkDisplay, i);
#endif
        if (screen == NULL)
            continue;

        // Find the X11 root window
        GdkWindow *gdkRoot = gdk_screen_get_root_window(screen);
        Window root = GDK_WINDOW_XID(gdkRoot);

        for (int i = 0; i < NUM_KEYS_TO_GRAB; ++i) {
            // Ignore the keys that we couldn't resolve
            KeyCode keycode = grabbed_keys[i];
            if (keycode == 0)
                continue;

            // Ungrab everything
#if GTK_CHECK_VERSION(3,22,0)
            gdk_x11_display_error_trap_push(gdkDisplay);
#else
            gdk_error_trap_push();
#endif
            XUngrabKey(dpy, keycode, AnyModifier, root);

#if GTK_CHECK_VERSION(3,22,0)
            gdk_display_flush(gdkDisplay);

            if (gdk_x11_display_error_trap_pop(gdkDisplay))
#else
            gdk_flush();

            if (gdk_error_trap_pop())
#endif
                g_printerr("Failed to ungrab %s\n", keysym_names[i]);
        }

        // Unregister for X events
        gdk_window_remove_filter(gdkRoot, filter_func, mis);
    }
}
