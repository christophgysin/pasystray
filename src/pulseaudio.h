/***
  This file is part of PaSystray

  Copyright 2011 Christoph Gysin

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
void pulseaudio_prepare_context();
void pulseaudio_connect();
void pulseaudio_start();

void context_state_cb(pa_context* c, void* userdata);

void subscribed_cb(pa_context* c, int success, void* userdata);
void event_cb(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, void* userdata);

void add_server_cb(pa_context* c, const pa_server_info* i, void* userdata);
void change_default_item(menu_info_t* mi, const char* new_default);

void add_sink_cb(pa_context* c, const pa_sink_info* i, int is_last, void* userdata);
void remove_sink(menu_info_t* mi, uint32_t idx);

void add_source_cb(pa_context* c, const pa_source_info* i, int is_last, void* userdata);
void remove_source(menu_info_t* mi, uint32_t idx);

void add_sink_input_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata);
void remove_sink_input(menu_info_t* mi, uint32_t idx);

void add_source_output_cb(pa_context* c, const pa_source_output_info* i, int is_last, void* userdata);
void remove_source_input(menu_info_t* mi, uint32_t idx);

void quit(const char* msg);

#ifdef DEBUG
void print_event(pa_subscription_event_type_t t, uint32_t index);
#endif

#endif /* PASYSTRAY_PULSEAUDIO_H */
