#include "pulseaudio_action.h"
#include "menu_info.h"

extern pa_context* context;

void pulseaudio_set_sink(menu_info_item_t* mii)
{
    pa_operation_unref(pa_context_set_default_sink(context, mii->name, pulseaudio_success_cb, mii));
}

void pulseaudio_set_source(menu_info_item_t* mii)
{
    pa_operation_unref(pa_context_set_default_source(context, mii->name, pulseaudio_success_cb, mii));
}

void pulseaudio_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* mii = userdata;

    if(!success)
        fprintf(stderr, "failed to set %s \"%s\"!\n", menu_info_type_name(mii->menu_info->type), mii->name);
}

void pulseaudio_move_input_to_sink(menu_info_item_t* input, menu_info_item_t* sink)
{
    pa_operation_unref(pa_context_move_sink_input_by_index(context, input->index, sink->index, pulseaudio_move_success_cb, input));
}

void pulseaudio_move_output_to_source(menu_info_item_t* output, menu_info_item_t* source)
{
    pa_operation_unref(pa_context_move_source_output_by_index(context, output->index, source->index, pulseaudio_move_success_cb, output));
}

void pulseaudio_move_success_cb(pa_context *c, int success, void *userdata)
{
    menu_info_item_t* to = userdata;
    menu_info_item_t* from = to->menu_info->parent;

    if(!success)
        fprintf(stderr, "failed to move %s \"%s\" to %s \"%s\"!\n",
                menu_info_type_name(from->menu_info->type), from->name,
                menu_info_type_name(to->menu_info->type), to->name);
}
