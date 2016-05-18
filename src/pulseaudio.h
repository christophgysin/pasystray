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

#ifndef PASYSTRAY_PULSEAUDIO_H
#define PASYSTRAY_PULSEAUDIO_H

#include <pulse/pulseaudio.h>

#include "menu_info.h"

void pulseaudio_init(menu_infos_t* mis);
void pulseaudio_destroy();
void pulseaudio_prepare_context(menu_infos_t* mis);
void pulseaudio_connect(menu_infos_t* mis);
void pulseaudio_reconnect(menu_infos_t* mis);
void pulseaudio_start();

void pulseaudio_context_state_cb(pa_context* c, void* userdata);

void pulseaudio_subscribed_cb(pa_context* c, int success, void* userdata);
void pulseaudio_event_cb(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, void* userdata);

void pulseaudio_event_new(pa_subscription_event_type_t facility, uint32_t index, menu_infos_t* mis);
void pulseaudio_event_change(pa_subscription_event_type_t facility, uint32_t index, menu_infos_t* mis);
void pulseaudio_event_remove(pa_subscription_event_type_t facility, uint32_t index, menu_infos_t* mis);

void pulseaudio_change_default_item(menu_info_t* mi, const char* new_default);

void pulseaudio_server_init_cb(pa_context* c, const pa_server_info* i, void* userdata);
void pulseaudio_server_add_cb(pa_context* c, const pa_server_info* i, void* userdata);
void pulseaudio_server_change_cb(pa_context* c, const pa_server_info* i, void* userdata);
void pulseaudio_server_add(const pa_server_info* i, void* userdata, gboolean is_new);

void pulseaudio_sink_init_cb(pa_context* c, const pa_sink_info* i, int is_last, void* userdata);
void pulseaudio_sink_add_cb(pa_context* c, const pa_sink_info* i, int is_last, void* userdata);
void pulseaudio_sink_change_cb(pa_context* c, const pa_sink_info* i, int is_last, void* userdata);
void pulseaudio_sink_add(const pa_sink_info* i, int is_last, void* userdata, gboolean is_new);

void pulseaudio_source_init_cb(pa_context* c, const pa_source_info* i, int is_last, void* userdata);
void pulseaudio_source_add_cb(pa_context* c, const pa_source_info* i, int is_last, void* userdata);
void pulseaudio_source_change_cb(pa_context* c, const pa_source_info* i, int is_last, void* userdata);
void pulseaudio_source_add(const pa_source_info* i, int is_last, void* userdata, gboolean is_new);

void pulseaudio_sink_input_init_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata);
void pulseaudio_sink_input_add_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata);
void pulseaudio_sink_input_change_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata);
void pulseaudio_sink_input_add(const pa_sink_input_info* i, int is_last, void* userdata, gboolean is_new);

void pulseaudio_source_output_init_cb(pa_context* c, const pa_source_output_info* i, int is_last, void* userdata);
void pulseaudio_source_output_add_cb(pa_context* c, const pa_source_output_info* i, int is_last, void* userdata);
void pulseaudio_source_output_change_cb(pa_context* c, const pa_source_output_info* i, int is_last, void* userdata);
void pulseaudio_source_output_add(const pa_source_output_info* i, int is_last, void* userdata, gboolean is_new);

void pulseaudio_module_init_cb(pa_context* c, const pa_module_info* i, int is_last, void* userdata);
void pulseaudio_module_add_cb(pa_context* c, const pa_module_info* i, int is_last, void* userdata);
void pulseaudio_module_change_cb(pa_context* c, const pa_module_info* i, int is_last, void* userdata);
void pulseaudio_module_add(const pa_module_info* i, int is_last, void* userdata, gboolean is_new);

void pulseaudio_quit(const char* msg);

void pulseaudio_print_event(pa_subscription_event_type_t t, uint32_t index);

#endif /* PASYSTRAY_PULSEAUDIO_H */
