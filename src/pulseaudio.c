#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "pulseaudio.h"
#include "systray.h"

static pa_context* context = NULL;
static pa_threaded_mainloop* m = NULL;

void pulseaudio_connect()
{
    pa_mainloop_api* mainloop_api = NULL;
    char* server = NULL;

    if(!(m = pa_threaded_mainloop_new())) {
        fprintf(stderr, "pa_threaded_mainloop_new() failed.");
        quit();
    }

    mainloop_api = pa_threaded_mainloop_get_api(m);

    pa_proplist* proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "systray");

    if(!(context = pa_context_new_with_proplist(mainloop_api, NULL, proplist))) {
        fprintf(stderr, "pa_context_new() failed.");
        quit();
    }

    pa_context_set_state_callback(context, context_state_cb, NULL);
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

void context_state_cb(pa_context* c, void* userdata)
{
    switch(pa_context_get_state(c))
    {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY:
            pa_context_set_subscribe_callback(c, event_cb, NULL);
            pa_operation_unref(pa_context_subscribe(c,
                        PA_SUBSCRIPTION_MASK_SINK |
                        PA_SUBSCRIPTION_MASK_SOURCE |
                        PA_SUBSCRIPTION_MASK_SINK_INPUT |
                        PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT |
                        PA_SUBSCRIPTION_MASK_SERVER,
                        subscribed_cb, NULL));
            break;

        case PA_CONTEXT_TERMINATED:
            fprintf(stderr, "terminated\n");
            break;

        case PA_CONTEXT_FAILED:
        default:
            fprintf(stderr, "Connection failure: %s", pa_strerror(pa_context_errno(c)));
    }
}

void subscribed_cb(pa_context* c, int success, void* userdata)
{
    pa_operation_unref(pa_context_get_server_info(c, add_server_cb, u->sinks));
    pa_operation_unref(pa_context_get_sink_info_list(c, add_sink_cb, u->sinks));
    pa_operation_unref(pa_context_get_source_info_list(c, add_source_cb, u->sources));
    pa_operation_unref(pa_context_get_sink_input_info_list(c, add_sink_input_cb, u->sink_inputs));
    pa_operation_unref(pa_context_get_source_output_info_list(c, add_source_output_cb, u->source_outputs));
}

void event_cb(pa_context* c, pa_subscription_event_type_t t, uint32_t idx, void* userdata)
{
    pa_subscription_event_type_t type = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;
    pa_subscription_event_type_t facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;

    fprintf(stderr, "event %s on %s (%u)\n",
        (type == PA_SUBSCRIPTION_EVENT_NEW) ? "new" :
        (type == PA_SUBSCRIPTION_EVENT_CHANGE) ? "change" :
        (type == PA_SUBSCRIPTION_EVENT_REMOVE) ? "remove" :
        "unknown",
        (facility == PA_SUBSCRIPTION_EVENT_SINK) ? "sink" :
        (facility == PA_SUBSCRIPTION_EVENT_SOURCE) ? "source" :
        (facility == PA_SUBSCRIPTION_EVENT_SINK_INPUT) ? "sink-input" :
        (facility == PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT) ? "source-output" :
        (facility == PA_SUBSCRIPTION_EVENT_SERVER) ? "server" :
        "unknown",
        idx);

    switch(facility)
    {
        case PA_SUBSCRIPTION_EVENT_SERVER:
            switch(type)
            {
                case PA_SUBSCRIPTION_EVENT_NEW:
                    // add_server
                    break;
                case PA_SUBSCRIPTION_EVENT_REMOVE:
                    // remove_server
                    break;
                default:
                    break;
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SINK:
            switch(type)
            {
                case PA_SUBSCRIPTION_EVENT_NEW:
                    pa_operation_unref(pa_context_get_sink_info_by_index(c, idx, add_sink_cb, u->sinks));
                    break;
                case PA_SUBSCRIPTION_EVENT_REMOVE:
                    // remove_sink
                    break;
                default:
                    break;
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            switch(type)
            {
                case PA_SUBSCRIPTION_EVENT_NEW:
                    pa_operation_unref(pa_context_get_source_info_by_index(c, idx, add_source_cb, u->sources));
                    break;
                case PA_SUBSCRIPTION_EVENT_REMOVE:
                    // remove_source
                    break;
                default:
                    break;
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            switch(type)
            {
                case PA_SUBSCRIPTION_EVENT_NEW:
                    pa_operation_unref(pa_context_get_sink_input_info(c, idx, add_sink_input_cb, u->sink_inputs));
                    break;
                case PA_SUBSCRIPTION_EVENT_REMOVE:
                    // remove_sink_input
                    break;
                default:
                    break;
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            switch(type)
            {
                case PA_SUBSCRIPTION_EVENT_NEW:
                    pa_operation_unref(pa_context_get_source_output_info(c, idx, add_source_output_cb, u->source_outputs));
                    break;
                case PA_SUBSCRIPTION_EVENT_REMOVE:
                    // remove_source_output
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void add_server_cb(pa_context* c, const pa_server_info* i, void* userdata)
{
    systray_submenu_add_radio_item(u->servers, i->host_name);
}

void add_sink_cb(pa_context* c, const pa_sink_info* i, int is_last, void* userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    systray_submenu_add_radio_item(u->sinks, i->description);
}

void add_source_cb(pa_context* c, const pa_source_info* i, int is_last, void* userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get source information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    const char* class = pa_proplist_gets(i->proplist, PA_PROP_DEVICE_CLASS);

    if(class && (strcmp(class, "monitor") == 0))
        return;

    systray_submenu_add_radio_item(u->sources, i->description);
}

void add_sink_input_cb(pa_context* c, const pa_sink_input_info* i, int is_last, void* userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    const char* app_name = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_NAME);
    const char* app_icon = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_ICON_NAME);

    systray_submenu_add_menu_item(u->sink_inputs, app_name, app_icon);
}

void add_source_output_cb(pa_context* c, const pa_source_output_info* i, int is_last, void* userdata)
{
    if(is_last < 0)
    {
        fprintf(stderr, "Failed to get source information: %s", pa_strerror(pa_context_errno(c)));
        return;
    }

    if(is_last)
        return;

    const char* app_name = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_NAME);
    const char* app_icon = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_ICON_NAME);

    systray_submenu_add_menu_item(u->source_outputs, app_name ? app_name : i->name, app_icon);
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
