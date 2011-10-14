#ifndef PASYSTRAY_PULSEAUDIO_ACTION_H
#define PASYSTRAY_PULSEAUDIO_ACTION_H

#include <pulse/pulseaudio.h>
#include "menu_info.h"

void pulseaudio_set_sink(menu_info_item_t* mii);
void pulseaudio_set_source(menu_info_item_t* mii);
void pulseaudio_success_cb(pa_context *c, int success, void *userdata);

#endif /* PASYSTRAY_PULSEAUDIO_ACTION_H */
