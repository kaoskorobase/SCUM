/*	-*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
	vi: ts=4 noet sw=4:

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

	$Id: SCUM_FLApp.hh 8 2004-08-15 14:42:24Z steve $
*/


#include "SCUM_App.hh"
#include "SCUM_Class.hh"
#include "SCUM_GC.hh"

#include <Fl/Fl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SCUM_APP_IDLE_TIMEOUT 3.1415926535898
//#define SCUM_APP_IDLE_TIMEOUT 3141.5926535898

static float gIdleTimeout = SCUM_APP_IDLE_TIMEOUT;
static SCUM_ClassRegistry* gClassRegistry = 0;

static sw_result HOWL_API howl_publish_reply_func(
	sw_discovery,
	sw_discovery_oid,
	sw_discovery_publish_status status,
	sw_opaque
	)
{
	return SW_OKAY;
}

static bool howl_publish_port(sw_discovery session, const char* name, const char* type, uint16_t port)
{
	sw_discovery_oid oid;
	sw_result res;

	fprintf(stderr, "howl_publish_port: %s %s %u\n", name, type, port);

	res = sw_discovery_publish(
		session,
		0,							// interface
		name,						// name
		type,						// type
		0,							// domain (.local)
		0,							// host
		port,						// port
		0, 0,						// text records
		howl_publish_reply_func, 0,	// reply func
		&oid						// request id
	);

	return res == SW_OKAY;
}

static void howlTickCB(void* data)
{
	sw_ulong msecs = 10;
	sw_salt_step((sw_salt)data, &msecs);
	Fl::repeat_timeout(1.0, &howlTickCB, data);
}

SCUM_App::SCUM_App()
	: m_socket(-1),
	  m_howlSession(0)
{
	if (!gClassRegistry) {
		SCUM_ClassRegistry* reg = gClassRegistry = new SCUM_ClassRegistry();
		extern void SCUM_Object_Init(SCUM_ClassRegistry*);
		SCUM_Object_Init(reg);
		extern void SCUM_View_Init(SCUM_ClassRegistry*);
		SCUM_View_Init(reg);
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
	close(m_socket);
	sw_discovery_fina(m_howlSession);
}

void SCUM_App::dataAvailableCB(int fd, void* data)
{
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	int sock = accept(fd, (struct sockaddr*)&addr, &addrlen);
	if (sock == -1) {
		perror("accept");
		exit(1);
	}
	SCUM_Class* klass = gClassRegistry->lookupClass("_Client");
	SCUM_ArgStream args;
	SCUM_Client* client = new SCUM_Client(klass, args, sock);
	((SCUM_App*)data)->addClient(client);
}

void SCUM_App::timeoutCB(void* data)
{
	SCUM_App* self = (SCUM_App*)data;
	self->m_shouldBeRunning = !self->m_clients.empty();
}

void SCUM_App::initOSC(const char* address, uint16_t portStart)
{
	if (m_socket != -1) return;
	const uint16_t portMax = 0xFFFF;
	uint16_t port;
	int ret;
	ret = socket(AF_INET, SOCK_STREAM, 0);
	if (ret == -1) {
		perror("socket");
		exit(1);
	}
	m_socket = ret;
	for (port = portStart; port < portMax; port++) {
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);
		ret = bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
		if (ret != -1) break;
	}
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
	if (sw_discovery_init(&m_howlSession) != SW_OKAY) {
		fprintf(stderr, "sw_discovery_init failed\n");
		exit(1);
	}
	if (!howl_publish_port(m_howlSession, "SCUM", "_osc._tcp.", port)) {
		fprintf(stderr, "publishing OSC service failed\n");
		exit(1);
	}
	if (sw_discovery_salt(m_howlSession, &m_howlSalt) != SW_OKAY) {
		fprintf(stderr, "salt failed\n");
		exit(1);
	}
	Fl::add_timeout(1.0, &howlTickCB, m_howlSalt);
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
	/* TODO: command line args
	 *       bind address
	 *       port
	 *       idle timeout
	 */
	if (argc > 1) {
		gIdleTimeout = atof(argv[1]);
	}
	SCUM::GCInit();
	SCUM_App::instance().initOSC(0, 57130);
	SCUM_App::instance().run();
	exit(0);
}

// EOF