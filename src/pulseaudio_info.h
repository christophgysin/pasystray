/***
  This file is part of PaSystray

  Copyright (C) 2011-2016  Christoph Gysin

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

#ifndef PASYSTRAY_PULSEAUDIO_INFO_H
#define PASYSTRAY_PULSEAUDIO_INFO_H

#include <pulse/pulseaudio.h>

char* context_info_str(pa_context* c);
char* server_info_str(const pa_server_info* i);
char* sink_info_str(const pa_sink_info* i);
char* source_info_str(const pa_source_info* i);
char* input_info_str(const pa_sink_input_info* i);
char* output_info_str(const pa_source_output_info* i);
char* module_info_str(const pa_module_info* i);

#endif /* PASYSTRAY_PULSEAUDIO_INFO_H */
