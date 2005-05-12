/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vi: et sta sw=4:

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


#include "SCUM_Button.hh"
#include "SCUM_Client.hh"
#include "SCUM_GC.hh"
#include "SCUM_Menu.hh"
#include "SCUM_System.hh"

using namespace SCUM;

// =====================================================================
// SCUM_Toggle

SCUM_Toggle::SCUM_Toggle(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_View(klass, client, oid, args),
      m_value(false),
      m_pushed(false)
{
    layout().minSize = SCUM_Size(15, 15);
}

void SCUM_Toggle::drawView(const SCUM_Rect& damage)
{
    GCSave();
    if (m_pushed) {
        GCSetColor(fgColor());
        GCFillRect(bounds());
        GCDrawBeveledRect(bounds(), 1, true);
    } else {
        GCSetColor(bgColor());
        GCFillRect(bounds());
        GCDrawBeveledRect(bounds(), 1, false);
    }
    GCRestore();
    // 	if (m_value) {
    // 		float x1 = bounds().minX();
    // 		float y1 = bounds().minY();
    // 		float x2 = bounds().maxX();
    // 		float y2 = bounds().maxY();

    // 		// TODO: support different styles
    // 		setColor(fgColor());
    // // 		setLineStyle(1);
    // // 		drawLine(x1, y1, x2, y2);
    // // 		drawLine(x2, y1, x1, y2);
    // 		setColor(fgColor());
    // 		float inset = min(size().w, size().h) * 0.2;
    // 		drawRect(bounds().inset(inset));
    // 	}
}

bool SCUM_Toggle::mouseDown(int, const SCUM_Point&)
{
    m_pushed = true;
    refresh();
    return true;
}

void SCUM_Toggle::mouseUp(int, const SCUM_Point& where)
{
    if (contains(where)) {
        toggleValue(true);
    }
    m_pushed = m_value;
    refresh();
}

void SCUM_Toggle::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "value")) {
        //setBoolValue(slot, setValue(boolValue(slot), false));
        setValue(args.get_i(), false);
        refresh();
    } else {
        SCUM_View::setProperty(key, args);
    }
}

#if 0
void SCUM_Toggle::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (key == SCUM_Symbol::value) {
        setBoolValue(slot, m_value);
    } else {
        SCUM_View::getProperty(key, slot);
    }
}
#endif

bool SCUM_Toggle::setValue(bool value, bool doSend)
{
    if (value != m_value) {
        m_value = value;
        if (doSend) {
            OSC::StaticClientPacket<36> p;
            p.openMessage("/changed", 3);
            putId(p);
            p.putString("value");
            p.putFloat32(value);
            p.closeMessage();
            send(p);
        }
        return true;
    }
    return false;
}

bool SCUM_Toggle::toggleValue(bool send)
{
    return setValue(!m_value, send);
}

// =====================================================================
// SCUM_Bang

SCUM_Bang::SCUM_Bang(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_View(klass, client, oid, args),
      m_flashTime(0.1),
      m_timeStamp(0.0),
      m_active(false)
{
    layout().minSize = SCUM_Size(15, 15);
    startAnimation();
}

void SCUM_Bang::drawView(const SCUM_Rect& damage)
{
    GCSave();
    GCSetColor(bgColor());
    GCFillRect(bounds());
    GCDrawBeveledRect(bounds(), 1, false);
    SCUM_Rect r(bounds().inset(2));
    GCSetColor(fgColor());
    GCDrawArc(r, 0, 360);
    if (m_active) {
        GCSetColor(fgColor());
        GCFillArc(r, 0, 360);
    }
    GCRestore();
}

bool SCUM_Bang::mouseDown(int, const SCUM_Point&)
{
    bang(true);
    return false;
}

void SCUM_Bang::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "value")) {
        bang(false);
    } else if (equal(key, "flashTime")) {
        m_flashTime = args.get_f();
    } else {
        SCUM_View::setProperty(key, args);
    }
}

#if 0
void SCUM_Bang::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (key == SCUM_Symbol::value) {
        setBoolValue(slot, false);
    } else if (equal(key, "flashTime")) {
        setFloatValue(slot, m_flashTime);
    } else {
        SCUM_View::getProperty(key, slot);
    }
}
#endif

void SCUM_Bang::animate()
{
    if (m_active && ((SCUM::time() - m_timeStamp) > m_flashTime)) {
        m_active = false;
        refresh();
    }
}

void SCUM_Bang::bang(bool doSend)
{
    m_timeStamp = SCUM::time();
    if (!m_active) {
        m_active = true;
        refresh();
    }
    if (doSend) {
        OSC::StaticClientPacket<28> p;
        p.openMessage("/changed", 2);
        putId(p);
        p.putString("value");
        p.closeMessage();
        send(p);
    }
}

// =====================================================================
// SCUM_Button

SCUM_Button::SCUM_Button(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_View(klass, client, oid, args),
      m_padding(SCUM_Point(2, 2)),
      m_textAlign(kAlignC),
      m_value(0),
      m_pushed(false)
{
    m_font = getClient()->font();
}

void SCUM_Button::drawView(const SCUM_Rect& damage)
{
    GCSave();

    if (m_states.empty()) {
        GCSetColor(bgColor());
        GCFillRect(bounds());
        GCDrawBeveledRect(bounds(), 1, m_pushed);
    } else {
        State& state = m_states[m_value];
        if (state.bgColor.isTransparent())
            GCSetColor(bgColor());
        else
            GCSetColor(state.bgColor);
        GCFillRect(bounds());
        GCDrawBeveledRect(bounds(), 1, m_pushed);
        if (state.fgColor.isTransparent())
            GCSetColor(fgColor());
        else
            GCSetColor(state.fgColor);
        state.text.draw(bounds().inset(m_padding), makeAlign(m_textAlign));
    }

    GCRestore();
}

bool SCUM_Button::mouseDown(int, const SCUM_Point&)
{
    m_pushed = true;
    refresh();
    return true;
}

void SCUM_Button::mouseUp(int, const SCUM_Point& where)
{
    m_pushed = false;
    if (contains(where)) setValue(m_value+1, true);
    refresh();
}

void SCUM_Button::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "value")) {
        //setBoolValue(slot, setValue(intValue(slot), false));
        setValue(args.get_i(), false);
    } else if (equal(key, "font")) {
        m_font = SCUM_Font(args.get_s(), args.get_f());
        for (int i=0; i < m_states.size(); i++) {
            m_states[i].text.setFont(m_font);
        }
        updateLayout();
    } else if (equal(key, "xPadding")) {
        m_padding.x = max(3.f, args.get_f());
        updateLayout();
    } else if (equal(key, "yPadding")) {
        m_padding.y = max(3.f, args.get_f());
        updateLayout();
    } else if (equal(key, "textAlignment")) {
        m_textAlign = args.get_i();
        refresh();
    } else if (equal(key, "states")) {
        m_states.clear();
        int n = args.get_i();
        if (n > 0) {
            m_states.reserve(n);
            while (n--) stateValue(m_states, args);
        }
        updateLayout();
    } else {
        SCUM_View::setProperty(key, args);
    }
}

#if 0
void SCUM_Button::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (key == SCUM_Symbol::value) {
        setIntValue(slot, m_value);
    } else if (equal(key, "padding")) {
        setPointValue(slot, m_padding);
    } else if (equal(key, "textAlignment")) {
        setIntValue(slot, m_textAlign);
    } else {
        SCUM_View::getProperty(key, slot);
    }
}
#endif

SCUM_Size SCUM_Button::getMinSize()
{
    SCUM_Size size;
    for (int i=0; i < m_states.size(); i++) {
        size = size.max(m_states[i].text.extents().size);
    }
    return size.padded(m_padding);
}

void SCUM_Button::stateValue(StateArray& array, SCUM_ArgStream& args)
{
    array.push_back(State());
    State& state = array.back();
    state.text.setFont(m_font);
    state.text.setText(args.get_s());
    state.fgColor = colorValue(args);
    state.bgColor = colorValue(args);
}

bool SCUM_Button::setValue(int value, bool doSend)
{
    if (!m_states.empty()) {
        if (value < 0) value = m_states.size() - 1;
        else if (value >= m_states.size()) value = 0;
        //if (m_value != value) {
        m_value = value;
        if (doSend) {
            OSC::StaticClientPacket<36> p;
            p.openMessage("/changed", 3);
            putId(p);
            p.putString("value");
            p.putInt32(value);
            p.closeMessage();
            send(p);
        }
        refresh();
        return true;
        //}
    }
    return false;
}

// =====================================================================
// SCUM_Choice

static const SCUM_Size kIndicatorSize(10, 5);
static const SCUM_Size kIndicatorPadding(6, 4);

SCUM_Choice::SCUM_Choice(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_View(klass, client, oid, args),
      m_padding(SCUM_Point(2, 2)),
      m_textAlign(kAlignC),
      m_value(0),
      m_menu(0)
{
    m_font = getClient()->font();
}

SCUM_Choice::~SCUM_Choice()
{
    delete m_menu;
}

void SCUM_Choice::drawView(const SCUM_Rect& damage)
{
    SCUM_Rect r(bounds());

    GCSave();

    GCSetColor(bgColor());
    GCFillRect(bounds());
    GCDrawBeveledRect(r, 1, false);

    // draw triangular indicator
    SCUM_Point p1(r.x + r.w - kIndicatorSize.w - kIndicatorPadding.w,
            r.y + .5f * r.h - .5f * kIndicatorSize.h);
    SCUM_Point p2(p1.x + kIndicatorSize.w, p1.y);
    SCUM_Point p3(p1.x + floorf(.5f * kIndicatorSize.w),
            p1.y + kIndicatorSize.h);

    GCSetColor(fgColor());
    GCDrawPolygon(p1, p2, p3);

    if (!m_states.empty()) {
        r.w -= kIndicatorSize.w;
        m_states[m_value].draw(r.inset(m_padding), makeAlign(m_textAlign));
    }

    GCRestore();
}

bool SCUM_Choice::mouseDown(int state, const SCUM_Point& where)
{
    if (m_menu) setValue(m_menu->popup(where, m_value, true), true);
    return false;
}

void SCUM_Choice::contextMenu(int state, const SCUM_Point& where)
{
    mouseDown(state, where);
}

void SCUM_Choice::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "value")) {
        //setBoolValue(slot, setValue(intValue(slot), false));
        setValue(args.get_i(), false);
    } else if (equal(key, "font")) {
        m_font = SCUM_Font(args.get_s(), args.get_f());
        updateLayout();
    } else if (equal(key, "xPadding")) {
        m_padding.x = max(3.f, args.get_f());
        updateLayout();
    } else if (equal(key, "yPadding")) {
        m_padding.y = max(3.f, args.get_f());
        updateLayout();
    } else if (equal(key, "textAlignment")) {
        m_textAlign = args.get_i();
        refresh();
    } else if (equal(key, "states")) {
        m_states.clear();
        m_states.reserve(args.get_i());
        while (!args.atEnd()) {
            m_states.push_back(SCUM_Text(args.get_s()));
        }
        delete m_menu; m_menu = 0;
        if (!m_states.empty()) {
            SCUM_MenuItems items;
            items.reserve(m_states.size());
            for (size_t i=0; i < m_states.size(); i++) {
                items.push_back(SCUM_MenuItem(MenuAction, m_states[i].text()));
            }
            //m_menu = new SCUM_Menu(getClient(), kInvalidOID, items);
        }
        updateLayout();
    } else {
        SCUM_View::setProperty(key, args);
    }
}

#if 0
void SCUM_Choice::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (key == SCUM_Symbol::value) {
        setIntValue(slot, m_value);
    } else if (equal(key, "padding")) {
        setPointValue(slot, m_padding);
    } else if (equal(key, "textAlignment")) {
        setIntValue(slot, m_textAlign);
    } else {
        SCUM_View::getProperty(key, slot);
    }
}
#endif

SCUM_Size SCUM_Choice::getMinSize()
{
    SCUM_Size size;

    for (size_t i=0; i < m_states.size(); i++) {
        size = size.max(m_states[i].extents().size);
    }

    size = size.padded(m_padding);
    size.h = max(size.h, kIndicatorSize.h + 2.f * kIndicatorPadding.h);
    size.w += kIndicatorSize.w + 2.f * kIndicatorPadding.w;

    return size;
}

bool SCUM_Choice::setValue(int value, bool doSend)
{
    if ((value >= 0) && (value < m_states.size()) && (m_value != value)) {
        m_value = value;
        if (m_menu) m_menu->setItem(m_value);
        if (doSend) {
            OSC::StaticClientPacket<36> p;
            p.openMessage("/changed", 3);
            putId(p);
            p.putString("value");
            p.putInt32(value);
            p.closeMessage();
            send(p);
        }
        refresh();
        return true;
    }
    return false;
}

#include "SCUM_Class.hh"

void SCUM_Button_Init(SCUM_ClassRegistry* reg)
{
    new SCUM_ClassT<SCUM_Bang>(reg, "Bang", "View");
    new SCUM_ClassT<SCUM_Button>(reg, "Button", "View");
    new SCUM_ClassT<SCUM_Choice>(reg, "Choice", "View");
    new SCUM_ClassT<SCUM_Toggle>(reg, "Toggle", "View");
}

// EOF
