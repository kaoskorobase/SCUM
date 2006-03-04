/*  -*- mode: c++; indent-tabs-mode: t; c-basic-offset: 4 -*-
    vi: noet sta sw=4:

    SCUM. copyright (c) 2004, 2005 stefan kersten.

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


#ifndef SCUM_RENDEZVOUS_HH_INCLUDED
#define SCUM_RENDEZVOUS_HH_INCLUDED

#include <stdint.h>

enum {
    kSCUM_ProtoUDP,
    kSCUM_ProtoTCP
};

bool SCUM_RendezvousStart();
void SCUM_RendezvousStop();
bool SCUM_RendezvousPublish(const char* serviceName, int protocol, uint16_t port);

#endif // SCUM_RENDEZVOUS_HH_INCLUDED
