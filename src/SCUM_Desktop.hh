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


#ifndef SCUM_DESKTOP_HH_INCLUDED
#define SCUM_DESKTOP_HH_INCLUDED

#include "SCUM_Lang.hh"

class SCUM_Desktop : public SCUM_Object
{
public:
    static SCUM_Desktop& instance();

    virtual void destroy(bool now);

    const SCUM_Color& fgColor() const { return m_fgColor; }
    const SCUM_Color& bgColor() const { return m_bgColor; }
    const SCUM_Color& focusColor() const { return m_focusColor; }
    const SCUM_Font& font() const { return m_font; }

    void setProperty(const PyrSymbol* key, PyrSlot* slot);
    void getProperty(const PyrSymbol* key, PyrSlot* slot);

    // resources (destroyed on shutdown)
    void retain(SCUM_Object* obj);
    void release(SCUM_Object* obj);

private:
    SCUM_Desktop();
    virtual ~SCUM_Desktop();

    void destroyAll(bool now);

private:
    SCUM_ObjectList		m_resources;
    SCUM_Color			m_fgColor;
    SCUM_Color			m_bgColor;
    SCUM_Color			m_focusColor;
    SCUM_Font			m_font;
};

#endif // SCUM_DESKTOP_HH_INCLUDED
