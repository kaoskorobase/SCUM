/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vim: et sta sw=4:

    SCUM. copyright (c) 2004 stefan kersten.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    $Id$
*/


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "SCUM_Rendezvous.hh"

static const char* SCUM_ProtoString(int proto)
{
    switch (proto) {
        case kSCUM_ProtoUDP: return "_osc._udp";
        case kSCUM_ProtoTCP: return "_osc._tcp";
        default: return 0;
    }
}

#if HAVE_HOWL
# include <howl.h>
# include <FL/Fl.H>

static sw_discovery g_howlSession = 0;

static void howlTickCB(void* data)
{
    sw_ulong msecs = 0;
    sw_salt_step((sw_salt)data, &msecs);
    Fl::repeat_timeout(1.0, &howlTickCB, data);
}

static sw_result HOWL_API howlPublishReplyFunc(
        sw_discovery,
        sw_discovery_oid,
        sw_discovery_publish_status status,
        sw_opaque
        )
{
    return SW_OKAY;
}

bool SCUM_RendezvousStart()
{
    sw_salt salt;

    fprintf(stderr, "SCUM: starting rendezvous (howl)\n");

    if (sw_discovery_init(&g_howlSession) != SW_OKAY) {
        return false;
    }

    if (sw_discovery_salt(g_howlSession, &salt) != SW_OKAY) {
        SCUM_RendezvousStop();
        return false;
    }

    Fl::add_timeout(1.0, &howlTickCB, salt);
    return true;
}

void SCUM_RendezvousStop()
{
    if (g_howlSession) {
        sw_discovery_fina(g_howlSession);
        g_howlSession = 0;
    }
}

bool SCUM_RendezvousPublish(const char* name, int proto, uint16_t port)
{
    sw_discovery_oid oid;
    sw_result res;

    const char* type = SCUM_ProtoString(proto);
    if (!type) return false;

    res = sw_discovery_publish(
            g_howlSession,
            0,				// interface
            name,			// name
            type,			// type
            0,				// domain (.local)
            0,				// host
            port,			// port
            0, 0,			// text records
            howlPublishReplyFunc, 0,	// reply func
            &oid			// request id
            );

    fprintf(stderr, "SCUM_RendezvousPublish(howl): %s %s %d : %d\n", name, type, port, res == SW_OKAY);

    return res == SW_OKAY;
}

#elif defined(__APPLE__)
# include <CoreServices/CoreServices.h>
# include <pthread.h>

bool SCUM_RendezvousStart()
{
    fprintf(stderr, "SCUM: starting rendezvous (Apple/CoreServices)\n");
    return true;
}

void SCUM_RendezvousStop()
{
}

static void* netServiceRegisterThreadFunc(void* data)
{
    CFNetServiceRef netServiceRef = (CFNetServiceRef)data;
    CFNetServiceRegister(netServiceRef, NULL);
    return 0;
}

static bool netServiceRegister(CFNetServiceRef netServiceRef)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, 1);
    pthread_t thread;
    int err = pthread_create(
            &thread,
            &attr,
            netServiceRegisterThreadFunc,
            netServiceRef
            );
    return err == 0;
}

bool SCUM_RendezvousPublish(const char* cname, int proto, uint16_t port)
{
    const char* ctype = SCUM_ProtoString(proto);
    if (!ctype) return false;

    CFStringRef type = CFStringCreateWithCString(
            kCFAllocatorDefault,
            ctype,
            kCFStringEncodingUTF8
            );

    CFStringRef name = CFStringCreateWithCString(
            kCFAllocatorDefault,
            cname,
            kCFStringEncodingUTF8
            );

    CFNetServiceRef netServiceRef =  CFNetServiceCreate(
            NULL,                   // default allocator
            CFSTR(""),              // default domain
            type,
            name,
            port
            );

    fprintf(stderr, "SCUM: created net service\n");

    CFRelease(name);
    CFRelease(type);

    if (!netServiceRef)
        return false;

    return netServiceRegister(netServiceRef);
}
#else
bool SCUM_RendezvousStart()
{
    return true;
}

void SCUM_RendezvousStop()
{
}

bool SCUM_RendezvousPublish(const char*, int, uint16_t)
{
    return true;
}
#endif

// EOF
