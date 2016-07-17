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

#ifndef PASYSTRAY_SYSTRAY_IMPL_H
#define PASYSTRAY_SYSTRAY_IMPL_H

#include "menu_info.h"

systray_t systray_impl_create(menu_infos_t* mis);
void systray_impl_set_icon(systray_t systray, const char* icon_name);
void systray_impl_set_tooltip(systray_t systray, const char* markup);

#endif /* PASYSTRAY_SYSTRAY_IMPL_H */
