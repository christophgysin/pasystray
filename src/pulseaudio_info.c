/***
  This file is part of PaSystray

  Copyright (C) 2011-2013  Christoph Gysin

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

#include <glib.h>

#include "pulseaudio_info.h"

char* context_info_str(pa_context* c)
{
    return g_strdup_printf(
            "Server String: %s\n"
            "Library Protocol Version: %u\n"
            "Server Protocol Version: %u\n"
            "Is Local: %s\n"
            "Client Index: %u\n"
            "Tile Size: %zu",
            pa_context_get_server(c),
            pa_context_get_protocol_version(c),
            pa_context_get_server_protocol_version(c),
            pa_context_is_local(c) ? "yes" : "no",
            pa_context_get_index(c),
            pa_context_get_tile_size(c, NULL));
}

char* server_info_str(const pa_server_info* i)
{
    char ss[PA_SAMPLE_SPEC_SNPRINT_MAX];
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    pa_sample_spec_snprint(ss, sizeof(ss), &i->sample_spec);
    pa_channel_map_snprint(cm, sizeof(cm), &i->channel_map);

    return g_strdup_printf(
            "User Name: %s\n"
            "Host Name: %s\n"
            "Server Name: %s\n"
            "Server Version: %s\n"
            "Default Sample Specification: %s\n"
            "Default Channel Map: %s\n"
            "Default Sink: %s\n"
            "Default Source: %s\n"
            "Cookie: %04x:%04x",
            i->user_name,
            i->host_name,
            i->server_name,
            i->server_version,
            ss,
            cm,
            i->default_sink_name,
            i->default_source_name,
            i->cookie >> 16,
            i->cookie & 0xFFFFU);
}

char* sink_info_str(const pa_sink_info* i)
{
    static const char *state_table[] = {
        [1+PA_SINK_INVALID_STATE] = "n/a",
        [1+PA_SINK_RUNNING] = "RUNNING",
        [1+PA_SINK_IDLE] = "IDLE",
        [1+PA_SINK_SUSPENDED] = "SUSPENDED"
    };

    char s[PA_SAMPLE_SPEC_SNPRINT_MAX];
    char cv[PA_CVOLUME_SNPRINT_MAX];
    char cvdb[PA_SW_CVOLUME_SNPRINT_DB_MAX];
    char v[PA_VOLUME_SNPRINT_MAX];
    char vdb[PA_SW_VOLUME_SNPRINT_DB_MAX];
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    char* pl;

    char* str = g_strdup_printf(
            "Sink #%u\n"
            "State: %s\n"
            "Name: %s\n"
            "Description: %s\n"
            "Driver: %s\n"
            "Sample Specification: %s\n"
            "Channel Map: %s\n"
            "Owner Module: %u\n"
            "Mute: %s\n"
            "Volume: %s%s%s\n"
            "Balance: %0.2f\n"
            "Base Volume: %s%s%s%s\n"
            "Monitor Source: %s\n"
            "Latency: %0.0f usec, configured %0.0f usec\n"
            "Flags: %s%s%s%s%s%s%s\n"
            "Properties:\n\t\t%s",
            i->index,
            state_table[1+i->state],
            i->name,
            i->description,
            i->driver,
            pa_sample_spec_snprint(s, sizeof(s), &i->sample_spec),
            pa_channel_map_snprint(cm, sizeof(cm), &i->channel_map),
            i->owner_module,
            i->mute ? "yes" : "no",
            pa_cvolume_snprint(cv, sizeof(cv), &i->volume),
            i->flags & PA_SINK_DECIBEL_VOLUME ? "\n        " : "",
            i->flags & PA_SINK_DECIBEL_VOLUME ? pa_sw_cvolume_snprint_dB(cvdb, sizeof(cvdb), &i->volume) : "",
            pa_cvolume_get_balance(&i->volume, &i->channel_map),
            pa_volume_snprint(v, sizeof(v), i->base_volume),
            i->flags & PA_SINK_DECIBEL_VOLUME ? " (" : "",
            i->flags & PA_SINK_DECIBEL_VOLUME ? pa_sw_volume_snprint_dB(vdb, sizeof(vdb), i->base_volume) : "",
            i->flags & PA_SINK_DECIBEL_VOLUME ? ")" : "",
            i->monitor_source_name,
            (double) i->latency, (double) i->configured_latency,
            i->flags & PA_SINK_HARDWARE ? "HARDWARE " : "",
            i->flags & PA_SINK_NETWORK ? "NETWORK " : "",
            i->flags & PA_SINK_HW_MUTE_CTRL ? "HW_MUTE_CTRL " : "",
            i->flags & PA_SINK_HW_VOLUME_CTRL ? "HW_VOLUME_CTRL " : "",
            i->flags & PA_SINK_DECIBEL_VOLUME ? "DECIBEL_VOLUME " : "",
            i->flags & PA_SINK_LATENCY ? "LATENCY " : "",
            i->flags & PA_SINK_SET_FORMATS ? "SET_FORMATS " : "",
            pl = pa_proplist_to_string_sep(i->proplist, "\n\t\t"));

    pa_xfree(pl);

    return str;
}

char* source_info_str(const pa_source_info* i)
{
    static const char *state_table[] = {
        [1+PA_SOURCE_INVALID_STATE] = "n/a",
        [1+PA_SOURCE_RUNNING] = "RUNNING",
        [1+PA_SOURCE_IDLE] = "IDLE",
        [1+PA_SOURCE_SUSPENDED] = "SUSPENDED"
    };

    char s[PA_SAMPLE_SPEC_SNPRINT_MAX];
    char cv[PA_CVOLUME_SNPRINT_MAX];
    char cvdb[PA_SW_CVOLUME_SNPRINT_DB_MAX];
    char v[PA_VOLUME_SNPRINT_MAX];
    char vdb[PA_SW_VOLUME_SNPRINT_DB_MAX];
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    char *pl;

    char* str = g_strdup_printf(
            "Source #%u\n"
            "State: %s\n"
            "Name: %s\n"
            "Description: %s\n"
            "Driver: %s\n"
            "Sample Specification: %s\n"
            "Channel Map: %s\n"
            "Owner Module: %u\n"
            "Mute: %s\n"
            "Volume: %s%s%s\n"
            "Balance: %0.2f\n"
            "Base Volume: %s%s%s%s\n"
            "Monitor of Sink: %s\n"
            "Latency: %0.0f usec, configured %0.0f usec\n"
            "Flags: %s%s%s%s%s%s\n"
            "Properties:\n\t\t%s",
            i->index,
            state_table[1+i->state],
            i->name,
            i->description,
            i->driver,
            pa_sample_spec_snprint(s, sizeof(s), &i->sample_spec),
            pa_channel_map_snprint(cm, sizeof(cm), &i->channel_map),
            i->owner_module,
            i->mute ? "yes" : "no",
            pa_cvolume_snprint(cv, sizeof(cv), &i->volume),
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? "\n       " : "",
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? pa_sw_cvolume_snprint_dB(cvdb, sizeof(cvdb), &i->volume) : "",
            pa_cvolume_get_balance(&i->volume, &i->channel_map),
            pa_volume_snprint(v, sizeof(v), i->base_volume),
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? " (" : "",
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? pa_sw_volume_snprint_dB(vdb, sizeof(vdb), i->base_volume) : "",
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? ")" : "",
            i->monitor_of_sink_name ? i->monitor_of_sink_name : "n/a",
            (double) i->latency, (double) i->configured_latency,
            i->flags & PA_SOURCE_HARDWARE ? "HARDWARE " : "",
            i->flags & PA_SOURCE_NETWORK ? "NETWORK " : "",
            i->flags & PA_SOURCE_HW_MUTE_CTRL ? "HW_MUTE_CTRL " : "",
            i->flags & PA_SOURCE_HW_VOLUME_CTRL ? "HW_VOLUME_CTRL " : "",
            i->flags & PA_SOURCE_DECIBEL_VOLUME ? "DECIBEL_VOLUME " : "",
            i->flags & PA_SOURCE_LATENCY ? "LATENCY " : "",
            pl = pa_proplist_to_string_sep(i->proplist, "\n\t\t"));

    pa_xfree(pl);

    return str;
}

char* input_info_str(const pa_sink_input_info* i)
{
    char t[32];
    char k[32];
    char s[PA_SAMPLE_SPEC_SNPRINT_MAX];
    char cv[PA_CVOLUME_SNPRINT_MAX];
    char cvdb[PA_SW_CVOLUME_SNPRINT_DB_MAX];
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    char f[PA_FORMAT_INFO_SNPRINT_MAX];
    char *pl;

    g_snprintf(t, sizeof(t), "%u", i->owner_module);
    g_snprintf(k, sizeof(k), "%u", i->client);

    char* str = g_strdup_printf(
            "Sink Input #%u\n"
            "Driver: %s\n"
            "Owner Module: %s\n"
            "Client: %s\n"
            "Sink: %u\n"
            "Sample Specification: %s\n"
            "Channel Map: %s\n"
            "Format: %s\n"
            "Mute: %s\n"
            "Volume: %s\n"
            "        %s\n"
            "Balance: %0.2f\n"
            "Buffer Latency: %0.0f usec\n"
            "Sink Latency: %0.0f usec\n"
            "Resample method: %s\n"
            "Properties:\n\t\t%s",
            i->index,
            i->driver,
            i->owner_module != PA_INVALID_INDEX ? t : "n/a",
            i->client != PA_INVALID_INDEX ? k : "n/a",
            i->sink,
            pa_sample_spec_snprint(s, sizeof(s), &i->sample_spec),
            pa_channel_map_snprint(cm, sizeof(cm), &i->channel_map),
            pa_format_info_snprint(f, sizeof(f), i->format),
            i->mute ? "yes" : "no",
            pa_cvolume_snprint(cv, sizeof(cv), &i->volume),
            pa_sw_cvolume_snprint_dB(cvdb, sizeof(cvdb), &i->volume),
            pa_cvolume_get_balance(&i->volume, &i->channel_map),
            (double) i->buffer_usec,
            (double) i->sink_usec,
            i->resample_method ? i->resample_method : "n/a",
            pl = pa_proplist_to_string_sep(i->proplist, "\n\t\t"));

    pa_xfree(pl);

    return str;
}

char* output_info_str(const pa_source_output_info* i)
{
    char t[32];
    char k[32];
    char s[PA_SAMPLE_SPEC_SNPRINT_MAX];
    char cv[PA_CVOLUME_SNPRINT_MAX];
    char cvdb[PA_SW_CVOLUME_SNPRINT_DB_MAX];
    char cm[PA_CHANNEL_MAP_SNPRINT_MAX];
    char f[PA_FORMAT_INFO_SNPRINT_MAX];
    char *pl;

    g_snprintf(t, sizeof(t), "%u", i->owner_module);
    g_snprintf(k, sizeof(k), "%u", i->client);

    char* str = g_strdup_printf(
            "Source Output #%u\n"
            "Driver: %s\n"
            "Owner Module: %s\n"
            "Client: %s\n"
            "Source: %u\n"
            "Sample Specification: %s\n"
            "Channel Map: %s\n"
            "Format: %s\n"
            "Mute: %s\n"
            "Volume: %s\n"
            "        %s\n"
            "Balance: %0.2f\n"
            "Buffer Latency: %0.0f usec\n"
            "Source Latency: %0.0f usec\n"
            "Resample method: %s\n"
            "Properties:\n\t\t%s",
            i->index,
            i->driver,
            i->owner_module != PA_INVALID_INDEX ? t : "n/a",
            i->client != PA_INVALID_INDEX ? k : "n/a",
            i->source,
            pa_sample_spec_snprint(s, sizeof(s), &i->sample_spec),
            pa_channel_map_snprint(cm, sizeof(cm), &i->channel_map),
            pa_format_info_snprint(f, sizeof(f), i->format),
            i->mute ? "yes" : "no",
            pa_cvolume_snprint(cv, sizeof(cv), &i->volume),
            pa_sw_cvolume_snprint_dB(cvdb, sizeof(cvdb), &i->volume),
            pa_cvolume_get_balance(&i->volume, &i->channel_map),
            (double) i->buffer_usec,
            (double) i->source_usec,
            i->resample_method ? i->resample_method : "n/a",
            pl = pa_proplist_to_string_sep(i->proplist, "\n\t\t"));

    pa_xfree(pl);

    return str;
}

char* module_info_str(const pa_module_info* i)
{
    char *pl;

    char* str = g_strdup_printf(
            "Module #%u\n"
            "Name: %s\n"
            "Argument: %s\n"
            "Used: %u\n"
            "Properties:\n\t\t%s",
            i->index,
            i->name,
            i->argument,
            i->n_used,
            pl = pa_proplist_to_string_sep(i->proplist, "\n\t\t"));

    pa_xfree(pl);

    return str;
}
