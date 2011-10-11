#ifndef PASYSTRAY_PULSEAUDIO_INFO_H
#define PASYSTRAY_PULSEAUDIO_INFO_H

#include <pulse/pulseaudio.h>

char* context_info_str(pa_context* c);
char* server_info_str(const pa_server_info* i);
char* sink_info_str(const pa_sink_info* i);
char* source_info_str(const pa_source_info* i);
char* input_info_str(const pa_sink_input_info* i);
char* output_info_str(const pa_source_output_info* i);

#endif /* PASYSTRAY_PULSEAUDIO_INFO_H */
