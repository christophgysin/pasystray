#include "pulseaudio_action.h"
#include "menu_info.h"

extern pa_context* context;

void pulseaudio_set_sink(menu_info_item_t* mii)
{
    pa_context_set_default_sink(context, mii->name, pulseaudio_success_cb, mii);
}

void pulseaudio_set_source(menu_info_item_t* mii)
{
    pa_context_set_default_source(context, mii->name, pulseaudio_success_cb, mii);
}

void pulseaudio_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    if(!success)
        fprintf(stderr, "failed to set %s \"%s\"!\n", menu_info_type_name(mii->menu_info->type), mii->name);
}
