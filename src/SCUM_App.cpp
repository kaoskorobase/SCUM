/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vi: et sta sw=4:

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


#include "SCUM_App.hh"
#include "SCUM_Class.hh"
#include "SCUM_GC.hh"
#if 0
#include "SCUM_Rendezvous.hh"
#endif
#include "SCUM_Socket.hh"

#include <errno.h>
#include <unistd.h>
#include <FL/Fl.H>

#define SCUM_APP_IDLE_TIMEOUT 3.1415926535898

static float gIdleTimeout = SCUM_APP_IDLE_TIMEOUT;
static SCUM_ClassRegistry* gClassRegistry = 0;

SCUM_App::SCUM_App()
    : m_socket(-1)
{
#if 0
    if (!SCUM_RendezvousStart()) {
        throw std::runtime_error("couldn't start rendezvous");
    }
#endif
    if (!gClassRegistry) {
        SCUM_ClassRegistry* reg = gClassRegistry = new SCUM_ClassRegistry();
        extern void SCUM_Object_Init(SCUM_ClassRegistry*);
        SCUM_Object_Init(reg);
        extern void SCUM_View_Init(SCUM_ClassRegistry*);
        SCUM_View_Init(reg);
        extern void SCUM_Button_Init(SCUM_ClassRegistry*);
        SCUM_Button_Init(reg);
        extern void SCUM_Container_Init(SCUM_ClassRegistry*);
        SCUM_Container_Init(reg);
        extern void SCUM_Slider_Init(SCUM_ClassRegistry*);
        SCUM_Slider_Init(reg);
        extern void SCUM_Text_Init(SCUM_ClassRegistry*);
        SCUM_Text_Init(reg);
        extern void SCUM_GL_Init(SCUM_ClassRegistry*);
        SCUM_GL_Init(reg);
        extern void SCUM_ScrollView_Init(SCUM_ClassRegistry*);
        SCUM_ScrollView_Init(reg);
        extern void SCUM_Graph_Init(SCUM_ClassRegistry*);
        SCUM_Graph_Init(reg);
        extern void SCUM_Client_Init(SCUM_ClassRegistry*);
        SCUM_Client_Init(reg);
        extern void SCUM_Window_Init(SCUM_ClassRegistry*);
        SCUM_Window_Init(reg);
    }
}

SCUM_App::~SCUM_App()
{
    ::close(m_socket);
#if 0
    SCUM_RendezvousStop();
#endif
}

void SCUM_App::dataAvailableCB(int fd, void* data)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int sock = accept(fd, (struct sockaddr*)&addr, &addrlen);
    if (sock == -1) {
        perror("accept");
        exit(1);
    }
    SCUM_Class* klass = gClassRegistry->lookupClass("Client");
    SCUM_ArgStream args;
    SCUM_Client* client = new SCUM_Client(klass, args, sock);
    ((SCUM_App*)data)->addClient(client);
}

void SCUM_App::timeoutCB(void* data)
{
    SCUM_App* self = (SCUM_App*)data;
    self->m_shouldBeRunning = !self->m_clients.empty();
}

static int bind_port(int socket, uint16_t port)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_LOOPBACK;
    addr.sin_port = htons(port);

    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        return errno;
    }

    if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        return errno;
    }

    return 0;
}

/*
static int bind_port_range(int socket, uint16_t portStart, uint16_t portMax, bool wrap)
{
    uint16_t port;
    int err;

    if (portStart > portMax)
        return bind_port_range(socket, portMax, portStart, wrap);

    for (port = portStart; port < portMax; port++) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_LOOPBACK;
        addr.sin_port = htons(port);
        err = bind(socket, (struct sockaddr*)&addr, sizeof(addr));
        if (err != -1) break;
    }

    if (err == -1) {
        return wrap ? bind_port_range(socket, 1025, portStart, false) : errno;
    }

    return 0;
}
*/

void SCUM_App::initOSC(const char* address, uint16_t port)
{
    if (m_socket != -1) return;
    int ret;
    ret = socket(AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
        perror("socket");
        exit(1);
    }
    m_socket = ret;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (address) {
        struct hostent* he = gethostbyname(address);
        if (he) addr.sin_addr.s_addr = *(int32_t*)he->h_addr;
    }
    int opt = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    ret = bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(1);
    }
    ret = listen(m_socket, 8);
    if (ret == -1) {
        perror("listen");
        exit(1);
    }
    Fl::add_fd(m_socket, &dataAvailableCB, this);
#if 0
    if (!SCUM_RendezvousPublish("SCUM", kSCUM_ProtoTCP, port)) {
        fprintf(stderr, "publishing OSC service failed\n");
        exit(1);
    }
#endif
}

void SCUM_App::addClient(SCUM_Client* client)
{
    m_clients.push_back(client);
}

void SCUM_App::removeClient(SCUM_Client* client)
{
    SCUM_ObjectList::iterator it;
    it = std::find(m_clients.begin(), m_clients.end(), client);
    if (it != m_clients.end()) {
        m_clients.erase(it);
        maybeQuit();
    }
}

void SCUM_App::maybeQuit()
{
    if (m_clients.empty()) {
        //Fl::add_timeout(gIdleTimeout, &timeoutCB, this);
        m_shouldBeRunning = false;
    }
}

void SCUM_App::run()
{
    m_shouldBeRunning = true;
    while (m_shouldBeRunning) {
        if (Fl::wait(1.0) < 0) {
            m_shouldBeRunning = false;
        }
    }
    printf("Quitting due to lack of interest. Goodbye.\n");
}

SCUM_App& SCUM_App::instance()
{
    static SCUM_App s_instance;
    return s_instance;
}

int main(int argc, char** argv)
{
    char* address = 0;
    uint16_t port = 57130;
    if (argc > 1) {
        address = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    SCUM::GCInit();
    SCUM_App::instance().initOSC(address, port);
    SCUM_App::instance().run();
    exit(0);
}

// EOF
