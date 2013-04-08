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

#include "pulseaudio_action.h"

#include <pulse/volume.h>
#include <pulse/ext-device-manager.h>

#include "notify.h"
#include "x11-property.h"

extern pa_context* context;

void pulseaudio_set_default(menu_info_item_t* mii)
{
    pa_operation* o = NULL;

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            if(mii->index == 0)
                x11_property_del("PULSE_SERVER");
            else
                x11_property_set("PULSE_SERVER", mii->address);
            break;
        case MENU_SINK:
            o = pa_context_set_default_sink(context, mii->name,
                    pulseaudio_set_default_success_cb, mii);
            break;
        case MENU_SOURCE:
            o = pa_context_set_default_source(context, mii->name,
                    pulseaudio_set_default_success_cb, mii);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            /* nothing to do here */
            break;
    }
    if(o)
        pa_operation_unref(o);
}

void pulseaudio_set_default_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    if(!success)
        g_error("failed to set default to %s \"%s\"!\n",
                menu_info_type_name(mii->menu_info->type), mii->name);
}

void pulseaudio_move_input_to_sink(menu_info_item_t* input, menu_info_item_t* sink)
{
    pa_operation_unref(pa_context_move_sink_input_by_index(context, input->index,
                sink->index, pulseaudio_move_success_cb, input));
}

void pulseaudio_move_output_to_source(menu_info_item_t* output, menu_info_item_t* source)
{
    pa_operation_unref(pa_context_move_source_output_by_index(context, output->index,
                source->index, pulseaudio_move_success_cb, output));
}

void pulseaudio_move_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* to = userdata;
    menu_info_item_t* from = to->menu_info->parent;

    if(!success)
        g_error("failed to move %s '%s' to %s '%s'!\n",
                menu_info_type_name(from->menu_info->type), from->name,
                menu_info_type_name(to->menu_info->type), to->name);
}

void pulseaudio_rename(menu_info_item_t* mii, const char* name)
{
#ifdef DEBUG
    g_message("rename %s '%s' to '%s'",
            menu_info_type_name(mii->menu_info->type), mii->desc, name);
#endif
    char *key = g_markup_printf_escaped("%s:%s", menu_info_type_name(mii->menu_info->type), mii->name);

    pa_operation* o;
    if(!(o = pa_ext_device_manager_set_device_description(context, key, name, pulseaudio_rename_success_cb, mii))) {
        g_error("pa_ext_device_manager_set_device_description(context, %s, %s) failed", key, name);
        return;
    }
    pa_operation_unref(o);
}

void pulseaudio_rename_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    // TODO: try to autoload module-device-manager?
    if(!success)
        g_warning("failed to rename %s '%s'! module-device-manager loaded?\n",
                menu_info_type_name(mii->menu_info->type), mii->name);
}

void pulseaudio_volume(menu_info_item_t* mii, int inc)
{
#ifdef DEBUG
    g_message("pulseaudio_volume(%s, %i)", mii->name, inc);
#endif

    /* increment/decrement in 2% steps */
    pa_cvolume* volume;
    if(inc < 0)
        volume = pa_cvolume_dec(mii->volume, -inc * PA_VOLUME_NORM / 50);
    else if(inc > 0)
        volume = pa_cvolume_inc(mii->volume, inc * PA_VOLUME_NORM / 50);
    else
        return;

    pa_operation* o = NULL;

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            /* nothing to do here */
            break;
        case MENU_SINK:
            o = pa_context_set_sink_volume_by_index(context, mii->index,
                    volume, pulseaudio_set_volume_success_cb, mii);
            break;
        case MENU_SOURCE:
            o = pa_context_set_source_volume_by_index(context, mii->index,
                    volume, pulseaudio_set_volume_success_cb, mii);
            break;
        case MENU_INPUT:
            o = pa_context_set_sink_input_volume(context, mii->index,
                    volume, pulseaudio_set_volume_success_cb, mii);
            break;
        case MENU_OUTPUT:
            o = pa_context_set_source_output_volume(context, mii->index,
                    volume, pulseaudio_set_volume_success_cb, mii);
            break;
    }
    if(o)
        pa_operation_unref(o);
}

void pulseaudio_set_volume_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    if(!success)
    {
        g_error("failed to set volume for %s \"%s\"!\n",
                menu_info_type_name(mii->menu_info->type), mii->name);
        return;
    }

    pulseaudio_update_volume_notification(mii, 0);
}

void pulseaudio_update_volume_notification(menu_info_item_t* mii, int is_muting)
{
    char vol[PA_CVOLUME_SNPRINT_MAX];
    gchar* msg = g_strdup_printf("%s %s: %s%s",
                menu_info_type_name(mii->menu_info->type), mii->desc,
                pa_cvolume_snprint(vol, sizeof(vol), mii->volume),
                (is_muting ^ mii->mute) ? " [muted]" : "");

    if(!mii->notify)
        mii->notify = notify(msg, NULL, mii->icon);
    else
        notify_update(mii->notify, msg, NULL, mii->icon);

    g_free(msg);
}

void pulseaudio_toggle_mute(menu_info_item_t* mii)
{
#ifdef DEBUG
    g_message("pulseaudio_toggle_mute(%s)", mii->name);
#endif

    pa_operation* o = NULL;

    int mute = (mii->mute) ? 0 : 1;

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            /* nothing to do here */
            break;
        case MENU_SINK:
            o = pa_context_set_sink_mute_by_index(context, mii->index,
                    mute, pulseaudio_toggle_mute_success_cb, mii);
            break;
        case MENU_SOURCE:
            o = pa_context_set_source_mute_by_index(context, mii->index,
                    mute, pulseaudio_toggle_mute_success_cb, mii);
            break;
        case MENU_INPUT:
            o = pa_context_set_sink_input_mute(context, mii->index,
                    mute, pulseaudio_toggle_mute_success_cb, mii);
            break;
        case MENU_OUTPUT:
            o = pa_context_set_source_output_mute(context, mii->index,
                    mute, pulseaudio_toggle_mute_success_cb, mii);
            break;
    }
    if(o)
        pa_operation_unref(o);
}

void pulseaudio_toggle_mute_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    if(!success)
    {
        g_error("failed to toogle mute for %s \"%s\"!\n",
                menu_info_type_name(mii->menu_info->type), mii->name);
        return;
    }

    pulseaudio_update_volume_notification(mii, 1);
}
