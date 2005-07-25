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


#include "SCUM_Text.hh"
#include "SCUM_Client.hh"
#include "SCUM_GC.hh"

#include <string.h>

using namespace SCUM;

// =====================================================================
// SCUM_Label

SCUM_Label::SCUM_Label(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
	: SCUM_View(klass, client, oid, args),
	  m_padding(SCUM_Point(5, 5)),
	  m_textAlign(kAlignC)
{
	m_text.setFont(getClient()->font());
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

void SCUM_Label::setProperty(const char* key, SCUM_ArgStream& args)
{
	if (equal(key, "text")) {
		m_text.setText(args.get_s());
		printf("frozenSize %d needsUpdate %d\n", m_frozenSize.isEmpty(), layoutNeedsUpdate(getMinSize()));
		if (m_frozenSize.isEmpty() && layoutNeedsUpdate(getMinSize())) {
			updateLayout();
		} else {
			refresh();
		}
	} else if (equal(key, "textAlignment")) {
		m_textAlign = args.get_i();
		refresh();
	} else if (equal(key, "font")) {
		m_text.setFont(fontValue(args));
		updateLayout();
	} else if (equal(key, "xPadding")) {
		m_padding.x = max(2.f, args.get_f());
		updateLayout();
	} else if (equal(key, "yPadding")) {
		m_padding.y = max(2.f, args.get_f());
		updateLayout();
	} else if (equal(key, "frozen")) {
		if (args.get_i()) {
			m_frozenSize = minSize();
		} else {
			m_frozenSize = SCUM_Size();
		}
		updateLayout();
	} else if (equal(key, "minSize") && (args.peek() == 's')) { // overridden from SCUM_View
		layout().minSize = m_text.font().measure(args.get_s()).size;
		updateLayout();
	} else {
		SCUM_View::setProperty(key, args);
	} 
}

#if 0
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
#endif

SCUM_Size SCUM_Label::getMinSize()
{
	return m_frozenSize.isEmpty() ?
		m_text.extents().size.padded(m_padding) :
		m_frozenSize.max(SCUM_View::getMinSize());
}

// =====================================================================
// SCUM_StringEntry

SCUM_StringEntry::SCUM_StringEntry(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
	: SCUM_Label(klass, client, oid, args)
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

SCUM_List::SCUM_List(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
	: SCUM_ScrollView(klass, client, oid, args),
	  m_value(-1),
	  m_textAlign(kAlignW),
	  m_padding(4, 1)
{
	m_font = getClient()->font();
	m_itemSize.h = m_font.extents().height + 2.f * m_padding.y;
}

void SCUM_List::drawContent(const SCUM_Rect& damage)
{
	printf("SCUM::draw: %d\n", m_items.size());
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
		printf("SCUM: %s\n", m_items[i].c_str());

		if (GCIsClipped(itemBounds)) break;
		itemBounds.y += h;
	}

	GCRestore();
}

void SCUM_List::drawContentFocus(const SCUM_Rect& damage)
{
	SCUM_ScrollView::drawContentFocus(damage);
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

void SCUM_List::setProperty(const char* key, SCUM_ArgStream& args)
{
	if (equal(key, "value")) {
		setValue(args.get_i(), false);
	} else if (equal(key, "items")) {
		m_items.clear();
		m_items.reserve(args.get_i());
		while (!args.atEnd()) {
			m_items.push_back(args.get_s());
		}
		m_itemSize.w = 0.f;
		for (int i=0; i < m_items.size(); i++) {
			m_itemSize.w = max(m_itemSize.w, m_font.measure(m_items[i].c_str()).size.w);
		}
		m_itemSize.w += 2.f * m_padding.x;
		m_scrollStep = SCUM_Point(1, m_itemSize.h);
		updateContentSize();
	} else if (equal(key, "font")) {
		m_font = fontValue(args);
		m_itemSize.h = m_font.extents().height + 2.f * m_padding.y;
		updateContentSize();
	} else if (equal(key, "textAlignment")) {
		m_textAlign = clipAlign(args.get_i());
		refresh();
	} else if (equal(key, "fgColorSel")) {
		m_fgColorSel = colorValue(args);
		refresh();
	} else if (equal(key, "bgColorSel")) {
		m_bgColorSel = colorValue(args);
		refresh();
	} else if (equal(key, "xPadding")) {
		m_padding.x = max(0.f, args.get_f());
		updateContentSize();
	} else if (equal(key, "yPadding")) {
		m_padding.y = max(0.f, args.get_f());
		updateContentSize();
	} else {
		SCUM_ScrollView::setProperty(key, args);
	}
}

#if 0
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
#endif

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

#include "SCUM_Class.hh"

void SCUM_Text_Init(SCUM_ClassRegistry* reg)
{
	new SCUM_ClassT<SCUM_Label>(reg, "Label", "View");
	new SCUM_ClassT<SCUM_StringEntry>(reg, "StringEntry", "Label");
	new SCUM_ClassT<SCUM_List>(reg, "List", "ScrollView");
}

// EOF
