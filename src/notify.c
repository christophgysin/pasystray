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

#include "config.h"

#ifndef HAVE_NOTIFY

#include "notify.h"

void notify_initialize(){}
notify_handle_t notify(const char* msg, const char* body, const char* icon){ return 0; }
void notify_update(notify_handle_t h, const char* msg, const char* body, const char* icon){}

#else

#include <libnotify/notify.h>
typedef struct NotifyNotification* notify_handle_t;

void notify_initialize()
{
    notify_init(PACKAGE_NAME);
}

void notify_show(NotifyNotification* n)
{
    GError* error = NULL;

    if(!notify_notification_show(n, &error))
        g_warning("[notify] unable to show notification: %s: %s (%i)",
                g_quark_to_string(error->domain), error->message,
                error->code);
}

notify_handle_t notify(const char* msg, const char* body, const char* icon)
{
    NotifyNotification* n = notify_notification_new(msg, body, icon);
    notify_notification_set_urgency(n, NOTIFY_URGENCY_LOW);
    notify_notification_set_timeout(n, 2000);
    notify_show(n);
    return (notify_handle_t) n;
}

void notify_update(notify_handle_t h, const char* msg, const char* body, const char* icon)
{
    NotifyNotification* n = (NotifyNotification*) h;
    notify_notification_set_timeout(n, 2000); // reset time

    if(!notify_notification_update(n, msg, body, icon))
        g_error("[notify] invalid arguments passed to notify_update()");

    notify_show(n);
}

#endif
