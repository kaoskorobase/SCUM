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

	$Id: SCUM_Menu.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_Menu.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_View.hh"

#include <assert.h>

#include <PyrKernel.h>
#include <PyrObject.h>
#include <PyrSlot.h>

using namespace SCUM;

// =====================================================================
// SCUM_MenuItem

SCUM_MenuItem::SCUM_MenuItem(int type, const std::string& text)
	: m_type(type), m_text(text)
{ }

SCUM_MenuItem::SCUM_MenuItem(const SCUM_MenuItem& item)
	: m_type(item.m_type), m_text(item.m_text)
{ }

SCUM_MenuItem::SCUM_MenuItem(PyrSlot* spec)
{
	if (!isKindOfSlot(spec, class_array) || (spec->uo->size < 2))
		throw TypeError();

	m_type = charValue(spec->uo->slots+0);
	m_text = stringValue(spec->uo->slots+1);
}

// =====================================================================
// SCUM_MenuHandle

// SCUM_MenuHandle::SCUM_MenuHandle()
// { }

// SCUM_MenuHandle::~SCUM_MenuHandle()
// { }

// =====================================================================
// SCUM_Menu

SCUM_Menu::SCUM_Menu(PyrObject* obj, const SCUM_MenuItems& items)
	: SCUM_Object(obj),
	  m_handle(0),
	  m_item(-1)
{
	m_handle = SCUM::MenuHandle::create(items);
	assert(m_handle != 0);
	SCUM_Desktop::instance().retain(this);
}

SCUM_Menu::~SCUM_Menu()
{
	SCUM_Desktop::instance().release(this);
	delete m_handle;
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
// 	if (send) changed(SCUM_Symbol::willPopup);
	item = m_handle->popup(where, item);
	if (item >= 0) {
		m_item = item;
// 		if (send) changed(SCUM_Symbol::value);
	}
	return item;
}

void SCUM_Menu::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "item")) {
		m_item = intValue(slot);
	} else if (key == SCUM_Symbol::value) {
		setBoolValue(slot, setValue(m_item, boolValue(slot)));
	} else {
		SCUM_Object::setProperty(key, slot);
	}
}

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

// EOF
