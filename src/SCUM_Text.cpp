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

	$Id: SCUM_Text.cpp,v 1.3 2004/08/15 14:42:24 steve Exp $
*/


#include "SCUM_Text.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_GC.hh"
#include "SCUM_Symbol.hh"

#include <string.h>

#include <PyrKernel.h>
#include <PyrObject.h>
#include <PyrSlot.h>

using namespace SCUM;

// =====================================================================
// SCUM_Label

SCUM_Label::SCUM_Label(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_padding(SCUM_Point(5, 5)),
	  m_textAlign(kAlignC)
{
	m_text.setFont(desktop()->font());
}

void SCUM_Label::drawView(const SCUM_Rect& damage)
{
	const SCUM_Rect r(bounds());

	GCSave();

	if (!bgColor().isTransparent()) {
		GCSetColor(bgColor());
		GCFillRect(bounds());
	}

	GCSetColor(fgColor());
	m_text.draw(r.inset(m_padding), makeAlign(m_textAlign));

	GCRestore();
}

void SCUM_Label::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "text")) {
		size_t size;
		const char* str = stringValue(slot, size);
		m_text.setText(str, size);
		if (layoutNeedsUpdate(getMinSize())) {
			updateLayout();
		} else {
			refresh();
		}
	} else if (equal(key, "textAlignment")) {
		m_textAlign = intValue(slot);
		refresh();
	} else if (equal(key, "font")) {
		m_text.setFont(fontValue(slot));
		updateLayout();
	} else if (equal(key, "xPadding")) {
		m_padding.x = max(2., floatValue(slot));
		updateLayout();
	} else if (equal(key, "yPadding")) {
		m_padding.y = max(2., floatValue(slot));
		updateLayout();
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_Label::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "xPadding")) {
		setFloatValue(slot, m_padding.x);
	} else if (equal(key, "yPadding")) {
		setFloatValue(slot, m_padding.y);
	} else if (equal(key, "textAlignment")) {
		setIntValue(slot, m_textAlign);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

SCUM_Size SCUM_Label::getMinSize()
{
	return m_text.extents().size.padded(m_padding);
}

// =====================================================================
// SCUM_StringEntry

SCUM_StringEntry::SCUM_StringEntry(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Label(parent, obj)
{ }

void SCUM_StringEntry::drawView(const SCUM_Rect& damage)
{
	GCSave();

	GCSetColor(bgColor());
	GCFillRect(bounds());

	GCDrawBeveledRect(bounds(), 1, true);

	GCSetColor(fgColor());
	m_text.draw(bounds().inset(m_padding), makeAlign(m_textAlign));

	GCRestore();
}

bool SCUM_StringEntry::mouseDown(int state, const SCUM_Point& where)
{
	return sendMouseDown(state, where);
}

void SCUM_StringEntry::mouseMove(int state, const SCUM_Point& where)
{
	sendMouseMove(state, where);
}

void SCUM_StringEntry::mouseUp(int state, const SCUM_Point& where)
{
	sendMouseUp(state, where);
}

void SCUM_StringEntry::scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
	sendScrollWheel(state, where, delta);
}

// void SCUM_StringEntry::setProperty(const PyrSymbol* key, PyrSlot* slot)
// {
// 	if (equal(key, "sendMouse")) {
// 		m_sendMouse = boolValue(slot);
// 	} else if (equal(key, "sendScroll")) {
// 		m_sendScroll = boolValue(slot);
// 	} else {
// 		SCUM_Label::setProperty(key, slot);
// 	}
// }

// void SCUM_StringEntry::getProperty(const PyrSymbol* key, PyrSlot* slot)
// {
// 	if (equal(key, "sendMouse")) {
// 		setBoolValue(slot, m_sendMouse);
// 	} else if (equal(key, "sendScroll")) {
// 		setBoolValue(slot, m_sendScroll);
// 	} else {
// 		SCUM_Label::getProperty(key, slot);
// 	}
// }

// =====================================================================
// SCUM_List

SCUM_List::SCUM_List(SCUM_Container* parent, PyrObject* obj)
	: SCUM_ScrollView(parent, obj),
	  m_value(-1),
	  m_textAlign(kAlignW),
	  m_padding(4, 1)
{
	m_font = desktop()->font();
	m_itemSize.h = m_font.extents().height + 2.f * m_padding.y;
}

void SCUM_List::drawContent()
{
	if (m_items.empty()) return;

	GCSave();

	SCUM_Rect r(contentBounds());
	
	const int n = (int)m_items.size();
	const float h = m_itemSize.h;
	int i = clip((int)(scrollOffset().y / h), 0, (int)m_items.size() - 1);

	SCUM_Rect itemBounds(SCUM_Point(r.x, r.y + i * h), m_itemSize);

	// skip clipped items, starting from the first visible
	for (; i < n; i++) {
		if (!GCIsClipped(itemBounds)) break;
		itemBounds.y += h;
	}

	// draw items that are not clipped
	for (; i < n; i++) {
		if (i == m_value) {
			GCSetColor(m_bgColorSel);
			GCFillRect(
				itemBounds.x, itemBounds.y,
				max(viewPortBounds().w, itemBounds.w), itemBounds.h
				);
			GCSetColor(m_fgColorSel);
		} else {
			GCSetColor(fgColor());
		}

		m_font.draw(itemBounds.inset(m_padding), makeAlign(m_textAlign), m_items[i].c_str());

		if (GCIsClipped(itemBounds)) break;
		itemBounds.y += h;
	}

	GCRestore();
}

void SCUM_List::drawContentFocus()
{
	SCUM_ScrollView::drawContentFocus();
}

bool SCUM_List::mouseDownInContent(int, const SCUM_Point& where)
{
	int value = (int)((where.y - contentBounds().y) / m_itemSize.h);
	if ((value < 0) || (value >= (int)m_items.size())) {
		value = -1;
	}
	setValue(value, true);
	return false;
}

void SCUM_List::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::value) {
		setBoolValue(slot, setValue(intValue(slot), false));
	} else if (equal(key, "items")) {
		stringValues(slot, m_items);
		m_itemSize.w = 0.f;
		for (int i=0; i < m_items.size(); i++) {
			m_itemSize.w = max(m_itemSize.w, m_font.measure(m_items[i].c_str()).size.w);
		}
		m_itemSize.w += 2.f * m_padding.x;
		m_scrollStep = SCUM_Point(1, m_itemSize.h);
		updateContentSize();
	} else if (equal(key, "font")) {
		m_font = fontValue(slot);
		m_itemSize.h = m_font.extents().height + 2.f * m_padding.y;
		updateContentSize();
	} else if (equal(key, "textAlignment")) {
		m_textAlign = clipAlign(intValue(slot));
		refresh();
	} else if (equal(key, "fgColorSel")) {
		m_fgColorSel = colorValue(slot);
		refresh();
	} else if (equal(key, "bgColorSel")) {
		m_bgColorSel = colorValue(slot);
		refresh();
	} else if (equal(key, "xPadding")) {
		m_padding.x = max(0., floatValue(slot));
		updateContentSize();
	} else if (equal(key, "yPadding")) {
		m_padding.y = max(0., floatValue(slot));
		updateContentSize();
	} else {
		SCUM_ScrollView::setProperty(key, slot);
	}
}

void SCUM_List::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::value) {
		setIntValue(slot, m_value);
	} else if (equal(key, "padding")) {
		setPointValue(slot, m_padding);
	} else if (equal(key, "textAlignment")) {
		setIntValue(slot, m_textAlign);
	} else {
		SCUM_ScrollView::getProperty(key, slot);
	}
}

SCUM_Size SCUM_List::preferredViewPortSize()
{
	return SCUM_Size(0, m_itemSize.h);
}

void SCUM_List::updateContentSize()
{
	setContentSize(SCUM_Size(m_itemSize.w, m_itemSize.h * m_items.size()));
	refresh();
}

bool SCUM_List::setValue(int value, bool send)
{
	if (value != m_value) {
		m_value = value;
		if (send) doAction();
		scrollVisible(
			SCUM_Rect(
				contentBounds().x,
				contentBounds().y + value * m_itemSize.h,
				m_itemSize.w, m_itemSize.h
				),
			SCUM_Point()
			);
		refresh();
		return true;
	}
	return false;
}

// EOF
