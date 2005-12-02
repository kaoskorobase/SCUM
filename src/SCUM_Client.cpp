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


#include "SCUM_App.hh"
#include "SCUM_Client.hh"
#include "SCUM_Class.hh"
#include "SCUM_Socket.hh"
#include "SCUM_Util.hh"
#include "st.h"

#include <FL/Fl.H>
#include <stdexcept>

typedef SCUM_ObjectList::iterator SCUM_ObjectIter;
using namespace SCUM;

static st_table* gOSCMethodTable;
struct OSCMethod
{
    OSCMethod(SCUM_Client::OSCMethod m)
	: m_method(m)
    { }
    SCUM_Client::OSCMethod m_method;
};

static bool initOSCMethodTable()
{
    if (!gOSCMethodTable) {
	gOSCMethodTable = st_init_strtable();
	return true;
    }
    return false;
}

static void addOSCMethod(const char* path, SCUM_Client::OSCMethod m)
{
    st_insert(gOSCMethodTable, (st_data_t)path, (st_data_t)(new OSCMethod(m)));
}

static SCUM_Client::OSCMethod lookupOSCMethod(const char* path)
{
    st_data_t value;
    if (st_lookup(gOSCMethodTable, (st_data_t)path, &value))
	return ((OSCMethod*)value)->m_method;
    return 0;
}

SCUM_Client::SCUM_Client(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Object(klass, client, oid, args)
{
}

SCUM_Client::SCUM_Client(SCUM_Class* klass, SCUM_ArgStream& args, int socket)
    : SCUM_Object(klass, 0, 0, args),
      m_socket(socket),
      m_dumpOSC(false)
{
    m_objects = st_init_numtable();
    m_fgColor = SCUM_Color(0, 0, 0);
    m_bgColor = SCUM_Color(1, 1, 1);
    m_focusColor = SCUM_Color(0, 0, 1);
    m_font = SCUM_Font("Helvetica", 12);
    Fl::add_fd(m_socket, &dataAvailableCB, this);
    if (initOSCMethodTable()) {
	addOSCMethod("new", &SCUM_Client::osc_new);
	addOSCMethod("free", &SCUM_Client::osc_free);
	addOSCMethod("dumpOSC", &SCUM_Client::osc_dumpOSC);
    }
}

SCUM_Client::~SCUM_Client()
{
    Fl::remove_fd(m_socket);
    close(m_socket);
    SCUM_ObjectIter it = m_resources.begin();
    while (it != m_resources.end()) {
	delete (SCUM_Object*)*it++;
    }
    SCUM_App::instance().removeClient(this);
}

uint16_t SCUM_Client::getPort() const
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    if ((getsockname(getSocket(), (sockaddr*)&saddr, &len) == -1) || (len != sizeof(saddr)))
	return 0;
    return ntohs(saddr.sin_port);
}

uint16_t SCUM_Client::getPeerPort() const
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    if ((getpeername(getSocket(), (sockaddr*)&saddr, &len) == -1) || (len != sizeof(saddr)))
	return 0;
    return ntohs(saddr.sin_port);
}

void SCUM_Client::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "fgColor")) {
	m_fgColor = colorValue(args);
    } else if (equal(key, "bgColor")) {
	m_bgColor = colorValue(args);
    } else if (equal(key, "focusColor")) {
	m_focusColor = colorValue(args);
    } else if (equal(key, "font")) {
	m_font = fontValue(args);
    } else {
	SCUM_Object::setProperty(key, args);
    }
}

#if 0
void SCUM_Client::getProperty(const char* key, lo_message m)
{
    /*
      if (equal(key, "bounds")) {
      setRectValue(slot, SCUM_Rect(SCUM_Point(), SCUM::screenSize()));
      } else if (equal(key, "fgColor")) {
      setColorValue(slot, fgColor());
      } else if (equal(key, "bgColor")) {
      setColorValue(slot, bgColor());
      } else if (equal(key, "focusColor")) {
      setColorValue(slot, focusColor());
      } else {
      SCUM_Object::getProperty(key, slot);
      }
    */
}
#endif

void SCUM_Client::retain(SCUM_Object* obj)
{
    SCUM_ASSERT(std::find(m_resources.begin(), m_resources.end(), obj) == m_resources.end());
    m_resources.push_back(obj);
}

void SCUM_Client::release(SCUM_Object* obj)
{
    if (!m_resources.empty()) {
	SCUM_ObjectIter it = find(m_resources.begin(), m_resources.end(), obj);
	if (it != m_resources.end()) {
	    m_resources.erase(it);
	}
    }
}

SCUM_Object* SCUM_Client::getObject(int oid)
{
    st_data_t value;
    if (st_lookup(m_objects, (st_data_t)oid, &value)) {
	return (SCUM_Object*)value;
    }
    return 0;
}

void SCUM_Client::send(const char* buffer, size_t size)
{
    const int32_t nsize = htonl(size);
    ::send(m_socket, &nsize, sizeof(nsize), 0);
    ::send(m_socket, buffer, size, 0);
}

void SCUM_Client::send(const OSC::ClientPacket& packet)
{
    send(packet.getData(), packet.getSize());
}

void SCUM_Client::message(const char* fmt, ...)
{
    const size_t bufferSize = 256;
    char buffer[bufferSize];
    va_list vargs;
    va_start(vargs, fmt); 
    vsnprintf(buffer, bufferSize, fmt, vargs);
    OSC::StaticClientPacket<512> p;
    p.openMessage("/message", 1);
    p.putString(buffer);
    p.closeMessage();
    send(p);
}

void SCUM_Client::dumpOSC(const char* path, SCUM_ArgStream args)
{
    printf("[ %s ", path);
    size_t size = args.size();
    size_t lastIndex = size - 1;
    for (int i=0; i < size; ++i) {
	switch (args.peek()) {
	case 'i': printf("%d", args.get_i()); break;
	case 'f': printf("%f", args.get_f()); break;
	case 's': printf("'%s'", args.get_s()); break;
	default: continue;
	}
	printf(i == lastIndex ? " ]\n" : " ");
    }
}

void SCUM_Client::error(SCUM_Object* who, const char* where, const char* fmt, ...)
{
    const size_t bufferSize = 256;
    char buffer[bufferSize];
    va_list vargs;
    va_start(vargs, fmt); 
    vsnprintf(buffer, bufferSize, fmt, vargs);
    OSC::StaticClientPacket<512> p;
    p.openMessage("/error", 3);
    if (who && who->getClass()) {
	p.putString(who->getClass()->getName());
    } else {
	p.putString("<unknown>");
    }
    p.putString(where);
    p.putString(buffer);
    p.closeMessage();
    send(p);
}

void SCUM_Client::addObject(int oid, SCUM_Object* obj)
{
    st_insert(m_objects, (st_data_t)oid, (st_data_t)obj);
}

void SCUM_Client::osc_new(const char* path, SCUM_ArgStream& args)
{
    const char* klassName = args.get_s();
    int oid = args.get_i();
    if (strncmp(klassName, "SCUM", 4) == 0) {
	klassName += 4;
    }
    SCUM_Class* klass = getClass()->getRegistry()->lookupClass(klassName);
    if (!klass) {
	throw std::runtime_error("class not found");
    }
    if (klass->isAbstract()) {
	throw std::runtime_error("abstract class");
    }
    if (getObject(oid)) {
	throw std::runtime_error("duplicate object id");
    }
    klass->makeObject(this, oid, args);
}

void SCUM_Client::osc_free(const char*, SCUM_ArgStream& args)
{
    st_data_t key = (st_data_t)args.get_i();
    st_data_t value;
    if (st_delete(m_objects, &key, &value)) {
	SCUM_Object* obj = (SCUM_Object*)value;
	delete obj;
    }
}

void SCUM_Client::osc_dumpOSC(const char*, SCUM_ArgStream& args)
{
    m_dumpOSC = (bool)args.get_i();
}

void SCUM_Client::dispatchMessage(char* path, char* data, size_t size)
{
    const char* methodName = *path == '/' ? path + 1 : path;
    SCUM_Object* recvr = 0;
    try {
	SCUM_ArgStream args(data, size);
	if (m_dumpOSC) dumpOSC(path, args);
	if (OSCMethod m = lookupOSCMethod(methodName)) {
	    (this->*m)(path, args);
	} else if ((args.size() > 0) && (args.peek() == 'i')) {
	    int oid = args.get_i();
	    recvr = oid == 0 ? this : getObject(oid);
	    if (!recvr) throw std::runtime_error("object not found");
	    SCUM_Class* klass = recvr->getClass();
	    if (!klass) throw std::runtime_error("classless object, wtf?");
	    klass->dispatchMethod(recvr, methodName, args);
	}
    } catch (OSC::Error& e) {
	error(recvr, path, "OSC Error: %s", e.what());
    } catch (std::runtime_error& e) {
	message(e.what());
    }
}

static const size_t gPacketBufferSize = 32768;
static float gPacketBuffer[gPacketBufferSize>>2];

void SCUM_Client::dataAvailableCB(int fd, void* data)
{
    SCUM_Client* self = (SCUM_Client*)data;
    int ret;
    try {
	int32_t size;
	ret = read(fd, &size, sizeof(size));
	if (ret != sizeof(size)) {
	    delete self;
	    return;
	}
	size = ntohl(size);
	if (size > 0) {
	    ret = read(fd, gPacketBuffer, size);
	    if (ret != size) {
		delete self;
		return;
	    }
	    OSC::ServerPacket packet((char*)gPacketBuffer, size);
	    OSC::MessageStream ms(packet);
	    while (!ms.atEnd()) {
		OSC::Message m = ms.next();
		self->dispatchMessage(m.path, m.data, m.size);
	    }
	}
    } catch (OSC::Error& e) {
	self->error(self, "<unknown>", "OSC Error: %s", e.what());
    }
}

void SCUM_Client_Init(SCUM_ClassRegistry* reg)
{
    SCUM_ClassT<SCUM_Client>* klass = new SCUM_ClassT<SCUM_Client>(reg, "Client", "Object", true);
}
