#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <pulse/pulseaudio.h>

void pulseaudio_connect();
void context_state_cb(pa_context *c, void *userdata);
void subscribed_cb(pa_context* c, int success, void* userdata);
void event_cb(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, void* userdata);
void add_sink_cb(pa_context *c, const pa_sink_info *i, int is_last, void *userdata);
void add_source_cb(pa_context *c, const pa_source_info *i, int is_last, void *userdata);
void quit();

#endif /* PULSEAUDIO_H */
