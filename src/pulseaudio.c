#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "pulseaudio.h"
#include "systray.h"

static pa_context *context = NULL;
static pa_threaded_mainloop *m = NULL;

void pulseaudio_connect()
{
    pa_mainloop_api *mainloop_api = NULL;
    char *server = NULL;

    if(!(m = pa_threaded_mainloop_new())) {
        fprintf(stderr, "pa_threaded_mainloop_new() failed.");
        quit();
    }

    mainloop_api = pa_threaded_mainloop_get_api(m);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "systray");

    if(!(context = pa_context_new_with_proplist(mainloop_api, NULL, proplist))) {
        fprintf(stderr, "pa_context_new() failed.");
        quit();
    }

    pa_context_set_state_callback(context, context_state_callback, NULL);
    if (pa_context_connect(context, server, 0, NULL) < 0) {
        fprintf(stderr, "pa_context_connect() failed: %s", pa_strerror(pa_context_errno(context)));
        quit();
    }

    if(pa_threaded_mainloop_start(m) < 0) {
        fprintf(stderr, "pa_threaded_mainloop_start() failed.");
        quit();
    }

    return;
}

void quit()
{
    if(context)
        pa_context_unref(context);

    if(m)
    {
        pa_signal_done();
        pa_threaded_mainloop_free(m);
    }
}

void context_state_callback(pa_context *c, void *userdata)
{
    switch(pa_context_get_state(c))
    {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY:
            pa_operation_unref(pa_context_get_sink_info_list(c, get_sink_info_callback, u->sinks));
            pa_operation_unref(pa_context_get_source_info_list(c, get_source_info_callback, u->sources));
            break;

        case PA_CONTEXT_TERMINATED:
            fprintf(stderr, "terminated\n");
            break;

        case PA_CONTEXT_FAILED:
        default:
            fprintf(stderr, "Connection failure: %s", pa_strerror(pa_context_errno(c)));
    }
}

void get_sink_info_callback(pa_context *c, const pa_sink_info *i, int is_last, void *userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    printf("Sink [%u]:   %s (%s)\n", i->index, i->description, i->name);
    systray_submenu_add_radio_item(u->sinks, i->description);
}

void get_source_info_callback(pa_context *c, const pa_source_info *i, int is_last, void *userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    printf("Source [%u]: %s (%s)\n", i->index, i->description, i->name);
    systray_submenu_add_radio_item(u->sources, i->description);
}

/*
static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t,
                              uint32_t idx, void *userdata)
{
    fd_info *i = userdata;
    pa_operation *o = NULL;

    if ((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK) {
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
            if (!(o = pa_context_get_sink_info_by_index(i->context, i->sink_index, sink_info_cb, i))) {
                debug(DEBUG_LEVEL_NORMAL, __FILE__": Failed to get sink info: %s", pa_strerror(pa_context_errno(i->context)));
                return;
            }
            debug(DEBUG_LEVEL_NORMAL, "source added: %s\n", "foo");
        }
    }

    pa_operation_unref(o);
}

static void subscribe_cb(pa_context *context, pa_subscription_event_type_t t,
                         uint32_t idx, void *userdata) {
    fd_info *i = userdata;
    pa_operation *o = NULL;

    if (i->sink_index != idx)
        return;

    if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) != PA_SUBSCRIPTION_EVENT_CHANGE)
        return;

    if (!(o = pa_context_get_sink_info_by_index(i->context, i->sink_index, sink_info_cb, i))) {
        debug(DEBUG_LEVEL_NORMAL, __FILE__": Failed to get sink info: %s", pa_strerror(pa_context_errno(i->context)));
        return;
    }

    pa_operation_unref(o);
}
*/





/*
            switch (action) {
                case STAT:
                    pa_operation_unref(pa_context_stat(c, stat_callback, NULL));
                    if (short_list_format)
                        break;
                    actions++;

                case INFO:
                    pa_operation_unref(pa_context_get_server_info(c, get_server_info_callback, NULL));
                    break;

                case PLAY_SAMPLE:
                    pa_operation_unref(pa_context_play_sample(c, sample_name, sink_name, PA_VOLUME_NORM, simple_callback, NULL));
                    break;

                case REMOVE_SAMPLE:
                    pa_operation_unref(pa_context_remove_sample(c, sample_name, simple_callback, NULL));
                    break;

                case UPLOAD_SAMPLE:
                    sample_stream = pa_stream_new(c, sample_name, &sample_spec, NULL);
                    pa_assert(sample_stream);

                    pa_stream_set_state_callback(sample_stream, stream_state_callback, NULL);
                    pa_stream_set_write_callback(sample_stream, stream_write_callback, NULL);
                    pa_stream_connect_upload(sample_stream, sample_length);
                    break;

                case EXIT:
                    pa_operation_unref(pa_context_exit_daemon(c, simple_callback, NULL));
                    break;

                case LIST:
                    if (list_type) {
                        if (pa_streq(list_type, "modules"))
                            pa_operation_unref(pa_context_get_module_info_list(c, get_module_info_callback, NULL));
                        else if (pa_streq(list_type, "sinks"))
                            pa_operation_unref(pa_context_get_sink_info_list(c, get_sink_info_callback, NULL));
                        else if (pa_streq(list_type, "sources"))
                            pa_operation_unref(pa_context_get_source_info_list(c, get_source_info_callback, NULL));
                        else if (pa_streq(list_type, "sink-inputs"))
                            pa_operation_unref(pa_context_get_sink_input_info_list(c, get_sink_input_info_callback, NULL));
                        else if (pa_streq(list_type, "source-outputs"))
                            pa_operation_unref(pa_context_get_source_output_info_list(c, get_source_output_info_callback, NULL));
                        else if (pa_streq(list_type, "clients"))
                            pa_operation_unref(pa_context_get_client_info_list(c, get_client_info_callback, NULL));
                        else if (pa_streq(list_type, "samples"))
                            pa_operation_unref(pa_context_get_sample_info_list(c, get_sample_info_callback, NULL));
                        else if (pa_streq(list_type, "cards"))
                            pa_operation_unref(pa_context_get_card_info_list(c, get_card_info_callback, NULL));
                        else
                            pa_assert_not_reached();
                    } else {
                        actions = 8;
                        pa_operation_unref(pa_context_get_module_info_list(c, get_module_info_callback, NULL));
                        pa_operation_unref(pa_context_get_sink_info_list(c, get_sink_info_callback, NULL));
                        pa_operation_unref(pa_context_get_source_info_list(c, get_source_info_callback, NULL));
                        pa_operation_unref(pa_context_get_sink_input_info_list(c, get_sink_input_info_callback, NULL));
                        pa_operation_unref(pa_context_get_source_output_info_list(c, get_source_output_info_callback, NULL));
                        pa_operation_unref(pa_context_get_client_info_list(c, get_client_info_callback, NULL));
                        pa_operation_unref(pa_context_get_sample_info_list(c, get_sample_info_callback, NULL));
                        pa_operation_unref(pa_context_get_card_info_list(c, get_card_info_callback, NULL));
                    }
                    break;

                case MOVE_SINK_INPUT:
                    pa_operation_unref(pa_context_move_sink_input_by_name(c, sink_input_idx, sink_name, simple_callback, NULL));
                    break;

                case MOVE_SOURCE_OUTPUT:
                    pa_operation_unref(pa_context_move_source_output_by_name(c, source_output_idx, source_name, simple_callback, NULL));
                    break;

                case LOAD_MODULE:
                    pa_operation_unref(pa_context_load_module(c, module_name, module_args, index_callback, NULL));
                    break;

                case UNLOAD_MODULE:
                    pa_operation_unref(pa_context_unload_module(c, module_index, simple_callback, NULL));
                    break;

                case SUSPEND_SINK:
                    if (sink_name)
                        pa_operation_unref(pa_context_suspend_sink_by_name(c, sink_name, suspend, simple_callback, NULL));
                    else
                        pa_operation_unref(pa_context_suspend_sink_by_index(c, PA_INVALID_INDEX, suspend, simple_callback, NULL));
                    break;

                case SUSPEND_SOURCE:
                    if (source_name)
                        pa_operation_unref(pa_context_suspend_source_by_name(c, source_name, suspend, simple_callback, NULL));
                    else
                        pa_operation_unref(pa_context_suspend_source_by_index(c, PA_INVALID_INDEX, suspend, simple_callback, NULL));
                    break;

                case SET_CARD_PROFILE:
                    pa_operation_unref(pa_context_set_card_profile_by_name(c, card_name, profile_name, simple_callback, NULL));
                    break;

                case SET_SINK_PORT:
                    pa_operation_unref(pa_context_set_sink_port_by_name(c, sink_name, port_name, simple_callback, NULL));
                    break;

                case SET_SOURCE_PORT:
                    pa_operation_unref(pa_context_set_source_port_by_name(c, source_name, port_name, simple_callback, NULL));
                    break;

                case SET_SINK_MUTE:
                    pa_operation_unref(pa_context_set_sink_mute_by_name(c, sink_name, mute, simple_callback, NULL));
                    break;

                case SET_SOURCE_MUTE:
                    pa_operation_unref(pa_context_set_source_mute_by_name(c, source_name, mute, simple_callback, NULL));
                    break;

                case SET_SINK_INPUT_MUTE:
                    pa_operation_unref(pa_context_set_sink_input_mute(c, sink_input_idx, mute, simple_callback, NULL));
                    break;

                case SET_SOURCE_OUTPUT_MUTE:
                    pa_operation_unref(pa_context_set_source_output_mute(c, source_output_idx, mute, simple_callback, NULL));
                    break;

                case SET_SINK_VOLUME:
                    if ((volume_flags & VOL_RELATIVE) == VOL_RELATIVE) {
                        pa_operation_unref(pa_context_get_sink_info_by_name(c, sink_name, get_sink_volume_callback, NULL));
                    } else {
                        pa_cvolume v;
                        pa_cvolume_set(&v, 1, volume);
                        pa_operation_unref(pa_context_set_sink_volume_by_name(c, sink_name, &v, simple_callback, NULL));
                    }
                    break;

                case SET_SOURCE_VOLUME:
                    if ((volume_flags & VOL_RELATIVE) == VOL_RELATIVE) {
                        pa_operation_unref(pa_context_get_source_info_by_name(c, source_name, get_source_volume_callback, NULL));
                    } else {
                        pa_cvolume v;
                        pa_cvolume_set(&v, 1, volume);
                        pa_operation_unref(pa_context_set_source_volume_by_name(c, source_name, &v, simple_callback, NULL));
                    }
                    break;

                case SET_SINK_INPUT_VOLUME:
                    if ((volume_flags & VOL_RELATIVE) == VOL_RELATIVE) {
                        pa_operation_unref(pa_context_get_sink_input_info(c, sink_input_idx, get_sink_input_volume_callback, NULL));
                    } else {
                        pa_cvolume v;
                        pa_cvolume_set(&v, 1, volume);
                        pa_operation_unref(pa_context_set_sink_input_volume(c, sink_input_idx, &v, simple_callback, NULL));
                    }
                    break;

                case SET_SOURCE_OUTPUT_VOLUME:
                    if ((volume_flags & VOL_RELATIVE) == VOL_RELATIVE) {
                        pa_operation_unref(pa_context_get_source_output_info(c, source_output_idx, get_source_output_volume_callback, NULL));
                    } else {
                        pa_cvolume v;
                        pa_cvolume_set(&v, 1, volume);
                        pa_operation_unref(pa_context_set_source_output_volume(c, source_output_idx, &v, simple_callback, NULL));
                    }
                    break;

                case SET_SINK_FORMATS:
                    set_sink_formats(c, sink_idx, formats);
                    break;

                case SUBSCRIBE:
                    pa_context_set_subscribe_callback(c, context_subscribe_callback, NULL);

                    pa_operation_unref(pa_context_subscribe(
                                              c,
                                              PA_SUBSCRIPTION_MASK_SINK|
                                              PA_SUBSCRIPTION_MASK_SOURCE|
                                              PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                              PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT|
                                              PA_SUBSCRIPTION_MASK_MODULE|
                                              PA_SUBSCRIPTION_MASK_CLIENT|
                                              PA_SUBSCRIPTION_MASK_SAMPLE_CACHE|
                                              PA_SUBSCRIPTION_MASK_SERVER|
                                              PA_SUBSCRIPTION_MASK_CARD,
                                              NULL,
                                              NULL));
                    break;

                default:
                    pa_assert_not_reached();
            }
            break;
        */

