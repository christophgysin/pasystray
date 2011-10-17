#ifndef PASYSTRAY_PULSEAUDIO_ACTION_H
#define PASYSTRAY_PULSEAUDIO_ACTION_H

#include <pulse/pulseaudio.h>
#include "menu_info.h"

void pulseaudio_set_sink(menu_info_item_t* mii);
void pulseaudio_set_source(menu_info_item_t* mii);
void pulseaudio_success_cb(pa_context *c, int success, void *userdata);

void pulseaudio_move_input_to_sink(menu_info_item_t* input, menu_info_item_t* sink);
void pulseaudio_move_output_to_source(menu_info_item_t* output, menu_info_item_t* source);
void pulseaudio_move_success_cb(pa_context *c, int success, void *userdata);

#endif /* PASYSTRAY_PULSEAUDIO_ACTION_H */
