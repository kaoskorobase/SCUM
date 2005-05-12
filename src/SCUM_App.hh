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


#ifndef SCUM_APP_HH_INCLUDED
#define SCUM_APP_HH_INCLUDED

#include "SCUM_Client.hh"
#include "SCUM_Object.hh"

#include <stdint.h>

class SCUM_App
{
public:
    static SCUM_App& instance();
    void initOSC(const char* addr, uint16_t port);

    void run();

private:
    SCUM_App();
    ~SCUM_App();

    friend class SCUM_Client;
    void addClient(SCUM_Client* obj);
    void removeClient(SCUM_Client* obj);

    void maybeQuit();

    static void dataAvailableCB(int, void*);
    static void timeoutCB(void*);

private:
    int			m_socket;
    SCUM_ObjectList	m_clients;
    bool		m_shouldBeRunning;
};

#endif // SCUM_APP_HH_INCLUDED
