/***
  This file is part of PaSystray

  Copyright (C) 2011-2015  Christoph Gysin

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

#ifndef PASYSTRAY_NOTIFY_H
#define PASYSTRAY_NOTIFY_H

#include "config.h"

typedef void* notify_handle_t;

void notify_initialize();
notify_handle_t notify(const char* msg, const char* body, const char* icon, gint value);
void notify_update(notify_handle_t h, const char* msg, const char* body, const char* icon, gint value);

#endif /* PASYSTRAY_NOTIFY_H */
