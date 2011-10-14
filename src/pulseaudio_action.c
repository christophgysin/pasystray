#include "pulseaudio_action.h"

extern pa_context* context;

void pulseaudio_set_sink(menu_info_item_t* mii)
{
    pa_context_set_default_sink(context, mii->name, pulseaudio_success_cb, NULL);
}

void pulseaudio_set_source(menu_info_item_t* mii)
{
    pa_context_set_default_source(context, mii->name, pulseaudio_success_cb, NULL);
}

void pulseaudio_success_cb(pa_context *c, int success, void *userdata)
{
    fprintf(stderr, "success: %i\n", success);
}
