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

#include "config.h"

#if ! HAVE_AVAHI

#include "avahi.h"

void avahi_init(GMainLoop* loop){}
void avahi_start(menu_infos_t* mis){}
void avahi_destroy(){}

#else

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/error.h>
#include <avahi-glib/glib-watch.h>
#include <avahi-glib/glib-malloc.h>

#include "avahi.h"

static const char* AVAHI_SERVICE_PULSEAUDIO_SERVER_TCP = "_pulse-server._tcp";

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

static void avahi_server_add(menu_info_t* mi, const char* name,
        const AvahiAddress* address, uint16_t port, AvahiStringList* txt);
static void avahi_server_remove(menu_info_t* mi, const char* name);

void avahi_init(GMainLoop* loop)
{
    avahi_set_allocator(avahi_glib_allocator());

    glib_poll = avahi_glib_poll_new(NULL, G_PRIORITY_DEFAULT);
    if(glib_poll == NULL)
    {
        g_warning("Failed to create Avahi glib poll object.");
        return;
    }
    poll_api = avahi_glib_poll_get(glib_poll);

    int error;
    client = avahi_client_new(poll_api, 0, avahi_client_callback, loop, &error);

    if(client == NULL)
    {
        g_warning("Error initializing Avahi: %s", avahi_strerror(error));
        return;
    }

    const char* version = avahi_client_get_version_string(client);
    if(version == NULL)
    {
        g_debug("Error getting version string: %s",
                avahi_strerror(avahi_client_errno(client)));
        return;
    }

    g_debug("Avahi Server Version: %s", version);
}

void avahi_start(menu_infos_t* mis)
{
    if(!client)
        return;

    sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
            AVAHI_SERVICE_PULSEAUDIO_SERVER_TCP, NULL, 0,
            avahi_browse_callback, mis);

    if(!sb)
        g_message("Failed to create service browser: %s",
                avahi_strerror(avahi_client_errno(client)));
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

static void avahi_client_callback(AvahiClient* c, AvahiClientState state, void* userdata)
{
    assert(c);

    if(state == AVAHI_CLIENT_FAILURE)
        g_message("Server connection failure: %s",
                avahi_strerror(avahi_client_errno(c)));
}

static void avahi_browse_callback(AvahiServiceBrowser* b, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiBrowserEvent event, const char* name,
        const char* type, const char* domain, AvahiLookupResultFlags flags,
        void* userdata)
 {
    assert(b);
    menu_infos_t* mis = userdata;

    switch (event)
    {
        case AVAHI_BROWSER_FAILURE:
            g_message("[avahi] %s",
                    avahi_strerror(avahi_client_errno(
                            avahi_service_browser_get_client(b))));
            return;

        case AVAHI_BROWSER_NEW:
            g_debug("[avahi] new service '%s' of type '%s' in domain '%s'",
                    name, type, domain);

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_service_resolver_new(
                            client, interface, protocol, name, type, domain,
                            AVAHI_PROTO_UNSPEC, 0, avahi_resolve_callback, userdata)))
                g_message("Failed to resolve service '%s': %s",
                        name, avahi_strerror(avahi_client_errno(client)));

            break;

        case AVAHI_BROWSER_REMOVE:
            avahi_server_remove(&mis->menu_info[MENU_SERVER], name);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            break;
    }
}

char* avahi_string_list_info(AvahiStringList* list)
{
    char* str = NULL;

    char* key = NULL;
    char* value = NULL;

    while(list)
    {
        avahi_string_list_get_pair(list, &key, &value, NULL);
        char* new = g_strdup_printf("%s%s%s: %s", str ? str : "", str ? "\n" : "", key, value);
        avahi_free(key);
        avahi_free(value);

        g_free(str);
        str = new;

        list = avahi_string_list_get_next(list);
    }

    return str;
}

static void avahi_resolve_callback(AvahiServiceResolver* r, AvahiIfIndex interface,
        AvahiProtocol protocol, AvahiResolverEvent event, const char* name,
        const char* type, const char* domain, const char* host_name,
        const AvahiAddress* address, uint16_t port, AvahiStringList* txt,
        AvahiLookupResultFlags flags, void* userdata)
{
    assert(r);
    menu_infos_t* mis = userdata;

    switch (event)
    {
        case AVAHI_RESOLVER_FAILURE:
            g_message("[avahi] Failed to resolve service '%s' of type '%s' in domain '%s': %s",
                    name, type, domain, avahi_strerror(avahi_client_errno(
                            avahi_service_resolver_get_client(r))));
            break;

        case AVAHI_RESOLVER_FOUND:
            avahi_server_add(&mis->menu_info[MENU_SERVER], name, address, port, txt);
            break;
    }

    avahi_service_resolver_free(r);
}

void avahi_server_add(menu_info_t* mi, const char* name, const AvahiAddress* address, uint16_t port, AvahiStringList* txt)
{
    char a[AVAHI_ADDRESS_STR_MAX];
    avahi_address_snprint(a, sizeof(a), address);

    gchar* host_port = g_strdup_printf("%s:%u", a, port);

    g_debug("[avahi] new server detected: %s %s",
            name, host_port);

    char* text = avahi_string_list_info(txt);

    // TODO: tooltip formatting
    gchar* tooltip = g_strdup_printf(
            "name: %s\n"
            "host: %s\n"
            "%s",
            name,
            host_port,
            text);

    // remove any existing entry detected from X property
    menu_info_item_t* mii = menu_info_item_get_by_desc(mi, host_port);
    if(mii)
        menu_info_item_remove_by_name(mi, mii->name);

    menu_info_item_update(mi, -1, name, name, NULL, 0, tooltip, NULL, host_port, -1);

    g_free(tooltip);
    avahi_free(text);
    g_free(host_port);
}

void avahi_server_remove(menu_info_t* mi, const char* name)
{
    g_debug("[avahi] remove server '%s'", name);

    menu_info_item_remove_by_name(mi, name);
}

#endif
