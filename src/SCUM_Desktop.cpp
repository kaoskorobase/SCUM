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


#include "SCUM_Desktop.hh"
#include "SCUM_System.hh"
#include "SCUM_Util.hh"

using namespace SCUM;

SCUM_Desktop::SCUM_Desktop()
    : SCUM_Object(0)
{
    m_fgColor = SCUM_Color(0, 0, 0);
    m_bgColor = SCUM_Color(1, 1, 1);
    m_focusColor = SCUM_Color(0, 0, 1);
    m_font = SCUM_Font("Helvetica", 12);
}

SCUM_Desktop::~SCUM_Desktop()
{
    destroyAll(true);
}

SCUM_Desktop& SCUM_Desktop::instance()
{
    static SCUM_Desktop s_instance;
    return s_instance;
}

void SCUM_Desktop::destroy(bool now)
{
    destroyAll(now);
    setPyrObj(0);
}

void SCUM_Desktop::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (equal(key, "fgColor")) {
	m_fgColor = colorValue(slot);
    } else if (equal(key, "bgColor")) {
	m_bgColor = colorValue(slot);
    } else if (equal(key, "focusColor")) {
	m_focusColor = colorValue(slot);
    } else if (equal(key, "font")) {
	m_font = fontValue(slot);
    } else {
	SCUM_Object::setProperty(key, slot);
    }
}

void SCUM_Desktop::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
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
}

void SCUM_Desktop::retain(SCUM_Object* obj)
{
    SCUM_ASSERT(std::find(m_resources.begin(), m_resources.end(), obj) == m_resources.end());
    m_resources.push_back(obj);
}

void SCUM_Desktop::release(SCUM_Object* obj)
{
    if (!m_resources.empty()) {
	SCUM_ObjectIter it = find(m_resources.begin(), m_resources.end(), obj);
	SCUM_ASSERT(it != m_resources.end());
	m_resources.erase(it);
    }
}

void SCUM_Desktop::destroyAll(bool now)
{
    SCUM_ObjectList list(m_resources);
    m_resources.clear();
    for (SCUM_ObjectIter it=list.begin(); it != list.end(); it++) {
	(*it)->destroy(now);
    }
}

// EOF
