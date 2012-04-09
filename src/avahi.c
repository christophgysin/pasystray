/***
  This file is part of PaSystray

  Copyright (C) 2011, 2012 Christoph Gysin

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

#ifndef HAVE_AVAHI

void avahi_init(GMainLoop* loop){}
void avahi_start(){}
void avahi_destroy(){}

#else

#include "avahi.h"

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/error.h>
#include <avahi-glib/glib-watch.h>
#include <avahi-glib/glib-malloc.h>

static AvahiGLibPoll* glib_poll = NULL;
static const AvahiPoll* poll_api = NULL;
static AvahiClient* client = NULL;
static AvahiServiceBrowser* sb = NULL;

static void avahi_client_callback(AvahiClient* c, AvahiClientState state, void*  userdata);
static void avahi_browse_callback(AvahiServiceBrowser* b, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiBrowserEvent event, const char* name,
        const char* type, const char* domain, AvahiLookupResultFlags flags,
        void* userdata);
static void avahi_resolve_callback(AvahiServiceResolver* r, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiResolverEvent event, const char* name,
        const char* type, const char* domain, const char* host_name,
        const AvahiAddress* address, uint16_t port, AvahiStringList* txt,
        AvahiLookupResultFlags flags, void* userdata);

void avahi_init(GMainLoop* loop)
{
    avahi_set_allocator(avahi_glib_allocator());

    glib_poll = avahi_glib_poll_new(NULL, G_PRIORITY_DEFAULT);
    if(glib_poll == NULL)
    {
        g_warning("Faild to create Avahi glib poll object.");
        return;
    }
    poll_api = avahi_glib_poll_get(glib_poll);

    int error;
    client = avahi_client_new(poll_api, 0, avahi_client_callback, loop, &error);

    if(client == NULL)
    {
        g_warning("Error initializing Avahi: %s", avahi_strerror (error));
        return;
    }

#ifdef DEBUG
    const char* version = avahi_client_get_version_string(client);
    if(version == NULL)
    {
        g_warning("Error getting version string: %s",
                avahi_strerror(avahi_client_errno(client)));
        return;
    }

    g_message("Avahi Server Version: %s", version);
#endif

    avahi_start();
}

void avahi_start()
{
    sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_http._tcp", NULL, 0, avahi_browse_callback, client);

    if(!sb)
        g_message("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(client)));
}

void avahi_destroy()
{
    if(sb)
        avahi_service_browser_free(sb);
    if(client)
        avahi_client_free(client);
    if(glib_poll)
        avahi_glib_poll_free(glib_poll);
}

static void avahi_client_callback(AvahiClient* c, AvahiClientState state, void*  userdata)
{
    assert(c);

    if(state == AVAHI_CLIENT_FAILURE)
        g_message("Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
}

static void avahi_browse_callback(AvahiServiceBrowser* b, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiBrowserEvent event, const char* name,
        const char* type, const char* domain, AvahiLookupResultFlags flags,
        void* userdata)
 {
    AvahiClient* c = userdata;
    assert(b);

    switch (event)
    {
        case AVAHI_BROWSER_FAILURE:
            g_message("[avahi] %s\n", avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
            return;

        case AVAHI_BROWSER_NEW:
            g_message("[avahi] NEW: service '%s' of type '%s' in domain '%s'\n", name, type, domain);

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_service_resolver_new(c, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, 0, avahi_resolve_callback, c)))
                g_message("Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_client_errno(c)));

            break;

        case AVAHI_BROWSER_REMOVE:
            g_message("[avahi] REMOVE: service '%s' of type '%s' in domain '%s'\n", name, type, domain);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            g_message("[avahi] %s\n", event == AVAHI_BROWSER_CACHE_EXHAUSTED ? "CACHE_EXHAUSTED" : "ALL_FOR_NOW");
            break;
    }
}

static void avahi_resolve_callback(AvahiServiceResolver* r, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiResolverEvent event, const char* name,
        const char* type, const char* domain, const char* host_name,
        const AvahiAddress* address, uint16_t port, AvahiStringList* txt,
        AvahiLookupResultFlags flags, void* userdata)
{
    assert(r);

    switch (event)
    {
        case AVAHI_RESOLVER_FAILURE:
            g_message("[avahi] Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
            break;

        case AVAHI_RESOLVER_FOUND:
        {
            g_message("Service '%s' of type '%s' in domain '%s':\n", name, type, domain);

            char a[AVAHI_ADDRESS_STR_MAX];
            avahi_address_snprint(a, sizeof(a), address);
            char* t = avahi_string_list_to_string(txt);

            g_message(
                    "\t%s:%u (%s)\n"
                    "\tTXT=%s\n"
                    "\tcookie is %u\n"
                    "\tis_local: %i\n"
                    "\tour_own: %i\n"
                    "\twide_area: %i\n"
                    "\tmulticast: %i\n"
                    "\tcached: %i\n",
                    host_name, port, a,
                    t,
                    avahi_string_list_get_service_cookie(txt),
                    !!(flags & AVAHI_LOOKUP_RESULT_LOCAL),
                    !!(flags & AVAHI_LOOKUP_RESULT_OUR_OWN),
                    !!(flags & AVAHI_LOOKUP_RESULT_WIDE_AREA),
                    !!(flags & AVAHI_LOOKUP_RESULT_MULTICAST),
                    !!(flags & AVAHI_LOOKUP_RESULT_CACHED));

            avahi_free(t);
        }
    }

    avahi_service_resolver_free(r);
}

#endif
