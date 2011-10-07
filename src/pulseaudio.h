#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <pulse/pulseaudio.h>

void pulseaudio_connect();
void quit();
void context_state_callback(pa_context *c, void *userdata);
void get_sink_info_callback(pa_context *c, const pa_sink_info *i, int is_last, void *userdata);
void get_source_info_callback(pa_context *c, const pa_source_info *i, int is_last, void *userdata);

#endif /* PULSEAUDIO_H */
