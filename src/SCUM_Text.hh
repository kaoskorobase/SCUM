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

	$Id: SCUM_Text.hh,v 1.3 2004/08/15 14:42:24 steve Exp $
*/


#ifndef SCUM_TEXT_HH_INCLUDED
#define SCUM_TEXT_HH_INCLUDED

#include "SCUM_View.hh"
#include "SCUM_ScrollView.hh"
#include "SCUM_Font.hh"
#include "SCUM_Image.hh"

#include <stdint.h>
#include <string>
#include <vector>

namespace SCUM
{
	const size_t maxStringSize = 256;
};

// =====================================================================
// SCUM_Label

class SCUM_Label : public SCUM_View
{
public:
	SCUM_Label(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView(const SCUM_Rect& damage);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual SCUM_Size getMinSize();

protected:
	SCUM_Point	m_padding;
	SCUM_Text	m_text;
	uint8_t		m_textAlign;
};

// =====================================================================
// SCUM_StringEntry

class SCUM_StringEntry : public SCUM_Label
{
public:
	SCUM_StringEntry(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView(const SCUM_Rect& damage);

	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void mouseMove(int state, const SCUM_Point& where);
	virtual void mouseUp(int state, const SCUM_Point& where);
	virtual void scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta);

// 	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
// 	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);
};

// =====================================================================
// SCUM_List

class SCUM_List : public SCUM_ScrollView
{
public:
	SCUM_List(SCUM_Container* parent, PyrObject* obj);

protected:
	virtual void drawContent();
	virtual void drawContentFocus();

	virtual bool mouseDownInContent(int, const SCUM_Point&);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

protected:
	virtual SCUM_Size preferredViewPortSize();

private:
	void updateContentSize();
	bool setValue(int value, bool send);
	
private:
	SCUM_Font					m_font;
	std::vector<std::string>	m_items;
	int							m_value;
	SCUM_Size					m_itemSize;
	int							m_textAlign;
	SCUM_Point					m_padding;
	SCUM_Color					m_fgColorSel;
	SCUM_Color					m_bgColorSel;
};

#endif // SCUM_TEXT_HH_INCLUDED
