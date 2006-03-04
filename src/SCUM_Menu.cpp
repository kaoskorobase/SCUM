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


#include "SCUM_Menu.hh"
#include "SCUM_Client.hh"
#include "SCUM_Util.hh"
#include "SCUM_View.hh"

using namespace SCUM;

// =====================================================================
// SCUM_MenuItem

SCUM_MenuItem::SCUM_MenuItem(int type, const std::string& text)
    : m_type(type), m_text(text)
{ }

SCUM_MenuItem::SCUM_MenuItem(const char* spec)
{
    m_type = *spec++;
    if (m_type) {
	m_text = spec;
    } else {
	m_type = kMenuSeparator;
	m_text = "";
    }
}

SCUM_MenuItem::SCUM_MenuItem(const SCUM_MenuItem& item)
    : m_type(item.m_type), m_text(item.m_text)
{ }

// =====================================================================
// SCUM_Menu

SCUM_Menu::SCUM_Menu(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Object(klass, client, oid, args),
      m_handle(0),
      m_item(-1)
{
    std::vector<SCUM_MenuItem> items;
    items.reserve(args.size());
    while (!args.atEnd()) {
	items.push_back(SCUM_MenuItem(args.get_s()));
    }
    setItems(items);
    getClient()->retain(this);
}

void SCUM_Menu::setItems(const std::vector<SCUM_MenuItem>& items)
{
    delete m_handle;
    m_handle = SCUM::MenuHandle::create(items);
    SCUM_ASSERT(m_handle != 0);    
}

SCUM_Menu::~SCUM_Menu()
{
    //SCUM_Desktop::instance().release(this);
    getClient()->release(this);
    m_handle->release();
}

bool SCUM_Menu::value(int item) const
{
    return m_handle->value(item);
}

bool SCUM_Menu::setValue(int item, bool value)
{
    bool prevValue = m_handle->value(item);
    if (prevValue != value) {
	m_handle->setValue(item, value);
	return true;
    }
    return false;
}

int SCUM_Menu::popup(const SCUM_Point& where, int item, bool send)
{
    //	if (send) changed(SCUM_Symbol::willPopup);
    item = m_handle->popup(where, item);
    if (item >= 0) {
	m_item = item;
	//		if (send) changed(SCUM_Symbol::value);
    }
    return item;
}

void SCUM_Menu::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "item")) {
	m_item = args.get_i();
    } else if (equal(key, "value")) {
	//setBoolValue(slot, setValue(m_item, boolValue(slot)));
	setValue(m_item, args.get_i());
    } else {
	SCUM_Object::setProperty(key, args);
    }
}

#if 0
void SCUM_Menu::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (key == SCUM_Symbol::value) {
	setBoolValue(slot, value(m_item));
    } else if (equal(key, "item")) {
	setIntValue(slot, m_item);
    } else {
	SCUM_Object::getProperty(key, slot);
    }
}
#endif

#include "SCUM_Class.hh"

void SCUM_Menu_Init(SCUM_ClassRegistry* reg)
{
    new SCUM_ClassT<SCUM_Menu>(reg, "Menu", "Object");
}

// EOF
