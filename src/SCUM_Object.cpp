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

	$Id: SCUM_Lang.hh 8 2004-08-15 14:42:24Z steve $
*/


#include "SCUM_Object.hh"
#include "SCUM_Client.hh"

SCUM_ArgStream::SCUM_ArgStream()
{ }

SCUM_ArgStream::SCUM_ArgStream(char* data, size_t size)
	: m_stream(data, size)
{ }

bool SCUM_ArgStream::atEnd() const
{
	return m_stream.atEnd();
}

size_t SCUM_ArgStream::size() const
{
	return m_stream.getSize();
}

int SCUM_ArgStream::peek() const
{
	return const_cast<SCUM_ArgStream*>(this)->m_stream.peekTag();
}

int32_t SCUM_ArgStream::get_i()
{
	return m_stream.getInt32();
}

float SCUM_ArgStream::get_f()
{
	return m_stream.getFloat32();
}

const char* SCUM_ArgStream::get_s()
{
	return m_stream.getString();
}

OSC::blob_t SCUM_ArgStream::get_b()
{
	return m_stream.getBlob();
}

// ====================================================================
// SCUM_Object

SCUM_Object::SCUM_Object(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
	: m_class(klass),
	  m_client(client),
	  m_id(oid)
{
	if (client && (oid >= SCUM_Client::kFirstOID)) {
		client->addObject(oid, this);
	}
}

SCUM_Object::~SCUM_Object()
{
}

void SCUM_Object::setProperty(const char*, SCUM_ArgStream&)
{
}

//void SCUM_Object::getProperty(const char*, lo_message)
//{
//}

void SCUM_Object::putId(OSC::ClientPacket& packet) const
{
	packet.putInt32(getId());
}

void SCUM_Object::send(const OSC::ClientPacket& packet) const
{
	getClient()->send(packet);
}

#include "SCUM_Class.hh"

void SCUM_Object::osc_set(const char*, SCUM_ArgStream& args)
{
	setProperty(args.get_s(), args);
}

void SCUM_Object_Init(SCUM_ClassRegistry* reg)
{
	SCUM_ClassT<SCUM_Object>* klass = new SCUM_ClassT<SCUM_Object>(reg, "Object", 0);
	klass->addMethod("set", &SCUM_Object::osc_set);
}

// EOF
