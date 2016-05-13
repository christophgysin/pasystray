/***
  This file is part of PaSystray

  Copyright (C) 2011-2015  Christoph Gysin

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

#ifndef PASYSTRAY_PULSEAUDIO_ACTION_H
#define PASYSTRAY_PULSEAUDIO_ACTION_H

#include <pulse/pulseaudio.h>
#include "menu_info.h"

void pulseaudio_set_default(menu_info_item_t* mii);
void pulseaudio_set_default_success_cb(pa_context *c, int success, void *userdata);

void pulseaudio_move_input_to_sink(menu_info_item_t* input, menu_info_item_t* sink);
void pulseaudio_move_output_to_source(menu_info_item_t* output, menu_info_item_t* source);
void pulseaudio_move_all(menu_info_item_t* mii);
void pulseaudio_move_success_cb(pa_context *c, int success, void *userdata);

void pulseaudio_move_all_inputs_to_sink(menu_info_item_t* sink);
void pulseaudio_move_all_inputs_to_sink_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata);

void pulseaudio_rename(menu_info_item_t* mii, const char* name);
void pulseaudio_rename_success_cb(pa_context *c, int success, void *userdata);

void pulseaudio_volume(menu_info_item_t* mii, int inc);
void pulseaudio_set_volume_success_cb(pa_context *c, int success, void *userdata);
void pulseaudio_update_volume_notification(menu_info_item_t* mii);

void pulseaudio_toggle_mute(menu_info_item_t* mii);
void pulseaudio_toggle_mute_success_cb(pa_context *c, int success, void *userdata);

void pulseaudio_module_load(const char* name, const char* argument);
void pulseaudio_module_load_success_cb(pa_context *c, uint32_t idx, void *userdata);

void pulseaudio_module_unload(menu_info_item_t* mii);
void pulseaudio_module_unload_success_cb(pa_context *c, int success, void *userdata);

#endif /* PASYSTRAY_PULSEAUDIO_ACTION_H */
