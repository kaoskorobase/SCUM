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

	$Id: SCUM_View.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_View.hh"
#include "SCUM_Container.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Menu.hh"
#include "SCUM_Window.hh"
#include "SCUM_System.hh"
#include "SCUM_Symbol.hh"

#include <assert.h>
#include <iostream>

#include <PyrSlot.h>
#include <PyrKernel.h>

using namespace SCUM;

// =====================================================================
// SCUM_View

SCUM_View::SCUM_View(SCUM_Container* parent, PyrObject* pyrObj)
	: SCUM_Object(pyrObj),
	  m_parent(0),
	  m_window(0),
	  m_animateTimer(0)
{
	memset(&m_flags, 0, sizeof(Flags));

	m_flags.vVisible = true;
	m_flags.vEnabled = true;
	m_flags.vCanFocus = true;

	if (parent) {
		parent->addChild(this);
		assert(m_window != 0);
	}
}

SCUM_View::~SCUM_View()
{
	stopAnimation();
	if (m_parent) m_parent->removeChild(this);
}

const SCUM_View* SCUM_View::parentView() const
{
	return static_cast<SCUM_View*>(m_parent);
}

SCUM_View* SCUM_View::parentView()
{
	return static_cast<SCUM_View*>(m_parent);
}

SCUM_Desktop* SCUM_View::desktop()
{
	return &SCUM_Desktop::instance();
}

void SCUM_View::raise()
{
	parent()->raise(this);
}

void SCUM_View::lower()
{
	parent()->lower(this);
}

SCUM_View* SCUM_View::viewAtPoint(const SCUM_Point& where)
{
	return (isVisible() && contains(where)) ? this : 0;
}

bool SCUM_View::mouseDown(int /* state */, const SCUM_Point& /* where */)
{
	return false;
}

void SCUM_View::mouseMove(int /* state */, const SCUM_Point& /* where */)
{
}

void SCUM_View::mouseUp(int /* state */, const SCUM_Point& /* where */)
{
}

void SCUM_View::scrollWheel(int /* state */, const SCUM_Point& /* where */, const SCUM_Point& /* delta */)
{
}

void SCUM_View::contextMenu(int state, const SCUM_Point& where)
{
	PyrSlot args[3];
	SetInt(args+0, state);
	SetFloat(args+1, where.x);
	SetFloat(args+2, where.y);
	sendMessage(SCUM_Symbol::contextMenu, 3, args);
}

void SCUM_View::keyDown(int state, wint_t key)
{
	PyrSlot args[3];
	SetInt(args+0, state);
	SetChar(args+1, (char)key);
	SetInt(args+2, key);
	sendMessage(SCUM_Symbol::keyDown, 3, args);
}

void SCUM_View::keyUp(int state, wint_t key)
{
	PyrSlot args[3];
	SetInt(args+0, state);
	SetChar(args+1, (char)key);
	SetInt(args+2, key);
	sendMessage(SCUM_Symbol::keyUp, 3, args);
}

void SCUM_View::refresh(const SCUM_Rect& damage)
{
	assert(m_window != 0);
	m_window->handle()->refresh(damage);
}

void SCUM_View::refresh()
{
	refresh(bounds());
}

void SCUM_View::draw(const SCUM_Rect& damage)
{
	if (isVisible() && !GCIsClipped(bounds())) {
		SCUM_Rect myDamage(bounds() & damage);
		drawView(myDamage);
		drawDisabled(myDamage);
		drawFocus(myDamage);
	}
}

void SCUM_View::drawView(const SCUM_Rect& damage)
{
	if (!bgColor().isTransparent()) {
		GCSave();
		GCSetColor(bgColor());
		GCFillRect(damage);
		GCRestore();
	}
}

void SCUM_View::drawDisabled(const SCUM_Rect& damage)
{
	if (!isEnabled()) {
		GCSetColor(bgColor().blend(SCUM_Color(0, 0, 0), 0.15));
		float x1 = bounds().minX() + 1;
		float x2 = bounds().maxX() - 1;
		float y1 = bounds().minY() + 1;
		float y2 = bounds().maxY() - 1;
		while (y1 < y2) {
			GCDrawLine(x1, y1, x2, y1);
			y1 += 2.f;
		}
	}
}

void SCUM_View::drawFocus(const SCUM_Rect& damage)
{
	drawFocusInBounds(bounds().inset(1));
}

void SCUM_View::initGL()
{
}

void SCUM_View::drawGL()
{
}

void SCUM_View::resizeGL(int w, int h)
{
}

bool SCUM_View::hasMouse() const
{
	assert(m_window != 0);
	return m_window->hasMouse(this);
}

bool SCUM_View::hasFocus() const
{
	assert(m_window != 0);
	return m_window->hasFocus(this);
}

bool SCUM_View::canFocus() const
{
	return flags().vCanFocus && isVisible() && isEnabled() && (parent() && parent()->canFocus());
}

void SCUM_View::makeFocus(bool flag, bool send)
{
	assert(m_window != 0);

	if (flag) {
		if (canFocus() && !hasFocus()) {
			m_window->unsetFocus(true);
			m_window->setFocusView(this);
			if (send) sendMessage(getsym("prHandleFocus"), 0, 0);
			refresh();
		}
	} else {
		if (hasFocus()) {
			m_window->setFocusView(0);
			if (send) sendMessage(getsym("prHandleFocus"), 0, 0);
			refresh();
		}
	}
}

SCUM_View* SCUM_View::nextFocus(bool canFocus, bool& foundFocus)
{
	if (hasFocus()) {
		foundFocus = true;
		return 0;
	}
	canFocus = canFocus && isEnabled() && isVisible() && flags().vCanFocus;
	if (canFocus && foundFocus) return this;
	return 0;
}

SCUM_View* SCUM_View::prevFocus(bool canFocus, bool& foundFocus)
{
	if (hasFocus()) {
		foundFocus = true;
		return 0;
	}
	canFocus = canFocus && isEnabled() && isVisible() && flags().vCanFocus;
	if (canFocus && foundFocus) return this;
	return 0;
}

bool SCUM_View::layoutNeedsUpdate(const SCUM_Size& newSize) const
{
	return (newSize.w > bounds().w) || (newSize.h > bounds().h);
}

void SCUM_View::updateLayout()
{
	if (parent()) parent()->updateLayout();
}

const SCUM_Size& SCUM_View::updatePreferredSize()
{
	layout().lastPreferredSize = preferredSize().max(layout().minSize);
	return layout().lastPreferredSize;
}

SCUM_Size SCUM_View::preferredSize()
{
	return layout().minSize;
}

void SCUM_View::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	assert(m_window != 0);

	if (equal(key, "visible")) {
		bool flag = boolValue(slot);
		SetFalse(slot);
		if (flags().vVisible != flag) {
			flags().vVisible = flag;
			bool focus = hasFocus();
			if (focus && !flag) {
				m_window->resetFocus(false);
				setBoolValue(slot, focus != hasFocus());
			}
			updateLayout();
		}
	} else if (equal(key, "enabled")) {
		bool flag = boolValue(slot);
		SetFalse(slot);
		if (flags().vEnabled != flag) {
			flags().vEnabled = flag;
			bool focus = hasFocus();
			if (focus && !flag) {
				m_window->resetFocus(false);
				setBoolValue(slot, focus != hasFocus());
			}
			refresh();
		}
	} else if (equal(key, "canFocus")) {
		bool flag = boolValue(slot);
		SetFalse(slot);
		if (flags().vCanFocus != flag) {
			flags().vCanFocus = flag;
			bool focus = hasFocus();
			if (focus && !flag) {
				m_window->resetFocus(false);
				setBoolValue(slot, focus != hasFocus());
			}
			refresh();
		}
	} else if (equal(key, "fgColor")) {
		m_fgColor = colorValue(slot);
		refresh();
	} else if (equal(key, "bgColor")) {
		m_bgColor = colorValue(slot);
		refresh();
	} else if (equal(key, "minSize")) {
		layout().minSize = sizeValue(slot);
		updateLayout();
	} else if (equal(key, "alignment")) {
		layout().alignment = checkAlign(intValue(slot));
		updateLayout();
	} else if (equal(key, "xExpand")) {
		layout().expand.x = max(0., floatValue(slot));
		updateLayout();
	} else if (equal(key, "yExpand")) {
		layout().expand.y = max(0., floatValue(slot));
		updateLayout();
	} else if (equal(key, "xFill")) {
		layout().fill.x = clip(floatValue(slot), 0., 1.);
		updateLayout();
	} else if (equal(key, "yFill")) {
		layout().fill.y = clip(floatValue(slot), 0., 1.);
		updateLayout();
	} else {
		SCUM_Object::setProperty(key, slot);
	}
}

void SCUM_View::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal("bounds", key)) {
		setRectValue(slot, bounds());
	} else if (equal("fgColor", key)) {
		setColorValue(slot, fgColor());
	} else if (equal("bgColor", key)) {
		setColorValue(slot, bgColor());
	} else if (equal("visible", key)) {
		setBoolValue(slot, isVisible());
	} else if (equal("enabled", key)) {
		setBoolValue(slot, isEnabled());
	} else if (equal("canFocus", key)) {
		setBoolValue(slot, canFocus());
	} else if (equal("hasFocus", key)) {
		setBoolValue(slot, hasFocus());
	} else if (equal("minSize", key)) {
		setSizeValue(slot, layout().minSize);
	} else if (equal("alignment", key)) {
		setIntValue(slot, layout().alignment);
	} else if (equal(key, "expand")) {
		setPointValue(slot, layout().expand);
	} else if (equal(key, "fill")) {
		setPointValue(slot, layout().fill);
	} else {
		SCUM_Object::getProperty(key, slot);
	}
}

void SCUM_View::setBounds(const SCUM_Rect& bounds)
{
	m_bounds = bounds;
	boundsChanged(bounds);
	refresh();
}

void SCUM_View::boundsChanged(const SCUM_Rect& /* bounds */)
{
}

void SCUM_View::doAction(PyrSymbol* message)
{
	sendMessage(message, 0, 0);
}

void SCUM_View::doAction()
{
	sendMessage(SCUM_Symbol::doAction, 0, 0);
}

void SCUM_View::animateAction(SCUM_Timer*)
{
	animate();
}

void SCUM_View::startAnimation()
{
	if (!m_animateTimer) {
		m_animateTimer
			= new SCUM_MemTimer<SCUM_View>(0.05, &SCUM_View::animateAction, this);
		addTimer(m_animateTimer);
	}
}

void SCUM_View::stopAnimation()
{
	if (m_animateTimer) {
		m_animateTimer->remove();
		m_animateTimer = 0;
	}
}

void SCUM_View::animate()
{
}

void SCUM_View::drawFocusInBounds(const SCUM_Rect& bounds)
{
	if (hasFocus() && window()->shouldDrawFocus()) {
		GCSave();
		GCSetColor(desktop()->focusColor());
		GCSetLineWidth(1);
		GCSetLineStyle(kLineStyleDot);
		GCDrawRect(bounds);
		GCRestore();
	}
}

bool SCUM_View::sendMouseDown(int state, const SCUM_Point& where)
{
	PyrSlot args[3];
	PyrSlot res;

	SetInt(args+0, state);
	SetFloat(args+1, where.x);
	SetFloat(args+2, where.y);

	sendMessage(SCUM_Symbol::mouseDown, 3, args, &res);

	return IsTrue(&res);
}

void SCUM_View::sendMouseMove(int state, const SCUM_Point& where)
{
	PyrSlot args[3];

	SetInt(args+0, state);
	SetFloat(args+1, where.x);
	SetFloat(args+2, where.y);

	sendMessage(SCUM_Symbol::mouseMove, 3, args, 0);
}

void SCUM_View::sendMouseUp(int state, const SCUM_Point& where)
{
	PyrSlot args[3];

	SetInt(args+0, state);
	SetFloat(args+1, where.x);
	SetFloat(args+2, where.y);

	sendMessage(SCUM_Symbol::mouseUp, 3, args, 0);
}

void SCUM_View::sendScrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
	PyrSlot args[5];

	SetInt(args+0, state);
	SetFloat(args+1, where.x);
	SetFloat(args+2, where.y);
	SetFloat(args+3, delta.x);
	SetFloat(args+4, delta.y);

	sendMessage(SCUM_Symbol::scrollWheel, 5, args, 0);
}

// EOF
