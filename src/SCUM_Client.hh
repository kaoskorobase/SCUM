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

	$Id$
*/


#ifndef SCUM_CLIENT_HH_INCLUDED
#define SCUM_CLIENT_HH_INCLUDED

#include "SCUM_Object.hh"
#include "st.h"

#include <OSC_Client.hh>
#include <stdarg.h>
#include <stdint.h>

class SCUM_Client : public SCUM_Object
{
public:
	typedef void (SCUM_Client::*OSCMethod)(const char* path, SCUM_ArgStream& args);

public:
	SCUM_Client(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	SCUM_Client(SCUM_Class* klass, SCUM_ArgStream& args, int socket);
	virtual ~SCUM_Client();

	inline int getSocket() const { return m_socket; }
	uint16_t getPort() const;
	uint16_t getPeerPort() const;

	const SCUM_Color& fgColor() const { return m_fgColor; }
	const SCUM_Color& bgColor() const { return m_bgColor; }
	const SCUM_Color& focusColor() const { return m_focusColor; }
	const SCUM_Font& font() const { return m_font; }

	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const char* key, lo_message m);

	void retain(SCUM_Object* obj);
	void release(SCUM_Object* obj);

	SCUM_Object* getObject(int oid);

	void send(const char* buffer, size_t size);
	void send(const OSC::ClientPacket& packet);

	void message(const char* fmt, ...);
	void error(SCUM_Object* who, const char* where, const char* fmt, ...);

private:
	friend class SCUM_Object;
	void addObject(int oid, SCUM_Object* obj);

	void dispatchMessage(char* path, char* data, size_t size);
	static void dataAvailableCB(int, void*);

	void osc_new(const char*, SCUM_ArgStream&);
	void osc_free(const char*, SCUM_ArgStream&);

private:
	int				m_socket;
	st_table*		m_objects;
	SCUM_ObjectList	m_resources;
	SCUM_Color		m_fgColor;
	SCUM_Color		m_bgColor;
	SCUM_Color		m_focusColor;
	SCUM_Font		m_font;
};

#endif // SCUM_CLIENT_HH_INCLUDED
