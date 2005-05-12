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


#ifndef SCUM_OBJECT_HH_INCLUDED
#define SCUM_OBJECT_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Color.hh"
#include "SCUM_Font.hh"

#include <OSC_Client.hh>
#include <OSC_Server.hh>

#include <list>

class SCUM_Class;
class SCUM_Client;

class SCUM_ArgStream
{
public:
	SCUM_ArgStream();
	SCUM_ArgStream(char* data, size_t size);

	bool atEnd() const;
	size_t size() const;
	int peek() const;
	int32_t get_i();
	float get_f();
	const char* get_s();
	OSC::blob_t get_b();

private:
	OSC::ArgStream m_stream;
};

class SCUM_Object
{
public:
	enum {
		kInvalidOID = -1,
		kRootOID = 0,
		kFirstOID
	};

public:
	SCUM_Object(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	virtual ~SCUM_Object();

	inline SCUM_Class* getClass() const { return m_class; }
	inline SCUM_Client* getClient() const { return m_client; }
	inline int getId() const { return m_id; }

	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const char* key, lo_message m);

	void putId(OSC::ClientPacket& packet) const;
	void send(const OSC::ClientPacket& packet) const;

	void osc_set(const char*, SCUM_ArgStream&);

private:
	SCUM_Class*		m_class;
	SCUM_Client*	m_client;
	int				m_id;
};

typedef std::list<SCUM_Object*> SCUM_ObjectList;
typedef SCUM_ObjectList::iterator SCUM_ObjectIter;

namespace SCUM
{
	inline bool equal(const char* s1, const char* s2)
	{
		return strcmp(s1, s2) == 0;
	}

	inline SCUM_Point pointValue(SCUM_ArgStream& args)
	{
		return SCUM_Point(args.get_f(), args.get_f());
	}
	inline SCUM_Size sizeValue(SCUM_ArgStream& args)
	{
		return SCUM_Size(args.get_f(), args.get_f());
	}
	inline SCUM_Rect rectValue(SCUM_ArgStream& args)
	{
		return SCUM_Rect(args.get_f(), args.get_f(), args.get_f(), args.get_f());
	}
	inline SCUM_Color colorValue(SCUM_ArgStream& args)
	{
		return SCUM_Color(args.get_f(), args.get_f(), args.get_f(), args.get_f());
	}
	inline SCUM_Font fontValue(SCUM_ArgStream& args)
	{
		return SCUM_Font(args.get_s(), args.get_f());
	}
};
#endif // SCUM_OBJECT_HH_INCLUDED
