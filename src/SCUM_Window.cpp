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

	$Id: SCUM_Window.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_Window.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Font.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"

#include <assert.h>
#include <algorithm>
#include <iostream>

#include <PyrSlot.h>

using namespace SCUM;

// =====================================================================
// SCUM_WindowHandle

// SCUM_WindowHandle::SCUM_WindowHandle(SCUM_Window* view)
// 	: m_view(view)
// {
// }

// SCUM_WindowHandle::~SCUM_WindowHandle()
// {
// }

// void SCUM_WindowHandle::destroyView()
// {
// 	delete m_view;
// 	m_view = 0;
// }

// =====================================================================
// SCUM_Window

SCUM_Window::SCUM_Window(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Bin(parent, obj),
	  m_deferredTimer(0),
	  m_handle(0),
	  m_mouseMoveView(0),
	  m_focusView(0)
{
	m_handle = SCUM::WindowHandle::create(this);
	m_window = this;

	flags().vVisible = m_handle->flags().visible;
	flags().wWasShown = false;
	flags().wResizable = true;
	flags().wHasFocus = false;

	memset(&m_deferredCommands, 0, sizeof(DeferredCommands));

	m_deferredTimer
		= new SCUM_MemTimer<SCUM_Window>(0.075, &SCUM_Window::deferredAction, this);
	addTimer(m_deferredTimer);

	desktop()->retain(this);
}

SCUM_Window::~SCUM_Window()
{
	desktop()->release(this);
	if (m_deferredTimer) {
		m_deferredTimer->remove();
		m_deferredTimer = 0;
	}
	// this is done in the language from closeEvent or destroy
// 	sendMessage(SCUM_Symbol::prDestroyed, 0, 0);
}

void SCUM_Window::destroy(bool now)
{
	if (now) {
		delete m_handle;
		m_handle = 0;
		delete this;
	} else {
		m_deferredCommands.destroy = true;
	}
}

void SCUM_Window::draw()
{
	SCUM_Rect damage(GCClippedRect(bounds()));
	std::cout << "SCUM_Window::draw: " << damage << "\n";
	static_cast<SCUM_View*>(this)->draw(damage);
}

void SCUM_Window::raise()
{
	m_deferredCommands.show = true;
	m_deferredCommands.hide = false;
}

void SCUM_Window::lower()
{
}

void SCUM_Window::closeEvent()
{
	sendMessage(getsym("prHandleClose"), 0, 0);
}

void SCUM_Window::showEvent()
{
	flags().vVisible = handle()->flags().visible;
}

void SCUM_Window::hideEvent()
{
	flags().vVisible = handle()->flags().visible;
}

void SCUM_Window::resizeEvent(const SCUM_Rect& bounds)
{
	m_screenBounds = bounds;
	setBounds(SCUM_Rect(0.f, 0.f, bounds.w, bounds.h));
}

void SCUM_Window::focusEvent(bool hasFocus)
{
// 	if (handle()->flags().focused != hasFocus) {
// 		flags().wHasFocus = hasFocus;
	if (m_focusView) m_focusView->refresh();
// 	}
}

void SCUM_Window::setMouseView(SCUM_View* view)
{
	assert(m_mouseMoveView == 0);
	m_mouseMoveView = view;
}

void SCUM_Window::mouseMove(int state, const SCUM_Point& where)
{
	if (m_mouseMoveView)
		m_mouseMoveView->mouseMove(state, where);
}

void SCUM_Window::mouseUp(int state, const SCUM_Point& where)
{
	if (m_mouseMoveView) {
		m_mouseMoveView->mouseUp(state, where);
		m_mouseMoveView = 0;
	}
}

void SCUM_Window::keyDown(int state, wint_t key)
{
	if (key == '\t') {
		if (state == 0) {
			tabNextFocus();
		} else if (state & kModMaskShift) {
			tabPrevFocus();
		}
	} else {
		if (m_focusView) {
			m_focusView->keyDown(state, key);
		} else {
			SCUM_Bin::keyDown(state, key);
		}
	}
}

void SCUM_Window::keyUp(int state, wint_t key)
{
	if (m_focusView) {
		m_focusView->keyUp(state, key);
	} else {
		SCUM_Bin::keyUp(state, key);
	}
}

void SCUM_Window::setFocusView(SCUM_View* view)
{
	m_focusView = view;
}

void SCUM_Window::unsetFocus(bool send)
{
	if (focusView()) focusView()->makeFocus(false, send);
}

void SCUM_Window::resetFocus(bool send)
{
	SCUM_View* view = focusView();
	if (view && !view->canFocus()) {
		view->makeFocus(false, send);
	}
}

void SCUM_Window::updateLayout()
{
	updateLayout(false, false);
}

void SCUM_Window::updateLayout(bool xFit, bool yFit)
{
	// defer layout updates
	m_deferredCommands.updateLayout = true;
	m_deferredCommands.xFit = m_deferredCommands.xFit || xFit;
	m_deferredCommands.yFit = m_deferredCommands.yFit || yFit;
}

SCUM_Size SCUM_Window::preferredSize()
{
	return SCUM_Bin::preferredSize().max(10);
}

void SCUM_Window::doUpdateLayout(bool xFit, bool yFit)
{
	SCUM_Size newSize = updatePreferredSize();
	SCUM_Size curSize = size();
	SCUM_Size minSize = newSize;

	if (!handle()->flags().shown) {
		newSize = newSize.max(m_initialSize);
	} else {
		if (!xFit) newSize.w = max(newSize.w, curSize.w);
		if (!yFit) newSize.h = max(newSize.h, curSize.h);
	}

// 	std::cout << "curSize " << curSize << " newSize " << newSize << "\n";

	// release constraints
	handle()->setMinSize(SCUM_Size());
	handle()->setMaxSize(SCUM_Size());

	// set size
	if (newSize == curSize) {
		setBounds(bounds());
	} else {
		handle()->setSize(newSize);
	}

	// enforce constraints
	if (flags().wResizable) {
		handle()->setMinSize(minSize);
		handle()->setMaxSize(SCUM_Size());
	} else {
		handle()->setMinSize(newSize);
		handle()->setMaxSize(newSize);
	}
}

void SCUM_Window::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "visible")) { // overridden from SCUM_View
		if (boolValue(slot)) {
			m_deferredCommands.hide = false;
			m_deferredCommands.show = true;
		} else {
			m_deferredCommands.show = false;
			m_deferredCommands.hide = true;
		}
	} else if (equal(key, "title")) {
		handle()->setTitle(stringValue(slot).c_str());
	} else if (equal(key, "initialSize")) {
		m_initialSize = sizeValue(slot);
	} else if (equal(key, "resizable")) {
		flags().wResizable = boolValue(slot);
		updateLayout(false, false);
	} else if (equal(key, "fullscreen")) {
		m_deferredCommands.fullscreen = boolValue(slot);
	} else if (equal(key, "decorated")) {
		WindowHandle::Flags flags(handle()->flags());
		flags.decorated = boolValue(slot);
		handle()->setFlags(flags);
	} else if (equal(key, "modal")) {
		WindowHandle::Flags flags(handle()->flags());
		flags.modal = boolValue(slot);
		handle()->setFlags(flags);
	} else {
		SCUM_Bin::setProperty(key, slot);
	}
}

void SCUM_Window::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "initialSize")) {
		setSizeValue(slot, m_initialSize);
	} else if (equal(key, "screenBounds")) {
		setRectValue(slot, m_screenBounds);
	} else if (equal(key, "resizable")) {
		setBoolValue(slot, flags().wResizable);
	} else if (equal(key, "fullscreen")) {
		setBoolValue(slot, handle()->flags().fullscreen);
	} else if (equal(key, "decorated")) {
		setBoolValue(slot, handle()->flags().decorated);
	} else if (equal(key, "modal")) {
		setBoolValue(slot, handle()->flags().modal);
	} else {
		SCUM_Bin::getProperty(key, slot);
	}
}

void SCUM_Window::deferredAction(SCUM_Timer*)
{
	if (m_deferredCommands.destroy) {
		m_deferredCommands.destroy = false;
		destroy(true);
		return;
	}

	if (handle()->flags().fullscreen != m_deferredCommands.fullscreen) {
		WindowHandle::Flags flags(handle()->flags());
		flags.fullscreen = m_deferredCommands.fullscreen;
		handle()->setFlags(flags);
		m_deferredCommands.fullscreen = handle()->flags().fullscreen;
	}

	if (m_deferredCommands.show) {
		m_deferredCommands.show = false;
		if (!handle()->flags().shown) {
			doUpdateLayout(false, false);
			m_deferredCommands.updateLayout = false;
		}
		handle()->show();
	} else if (m_deferredCommands.hide) {
		m_deferredCommands.hide = false;
		handle()->hide();
	}

	// currently unused
	// user resizes are propagated instantly
#if 0
	if (m_deferredCommands.resize) {
		m_deferredCommands.resize = false;
		SCUM_Rect newBounds(m_screenBounds);
		newBounds.x = newBounds.y = 0.0f;
// 		if (newBounds != bounds()) {
			setBounds(newBounds);
// 		}
	}
#endif // 0

	if (m_deferredCommands.updateLayout) {
		doUpdateLayout(
			m_deferredCommands.xFit,
			m_deferredCommands.yFit
			);
		m_deferredCommands.updateLayout = false;
		m_deferredCommands.xFit = false;
		m_deferredCommands.yFit = false;
	}
}

void SCUM_Window::tabNextFocus()
{
	bool foundFocus = !focusView();
	SCUM_View* focus = nextFocus(true, foundFocus);
	if (!focus && foundFocus) focus = nextFocus(true, foundFocus);
	if (focus) focus->makeFocus(true, true);
}

void SCUM_Window::tabPrevFocus()
{
	bool foundFocus = !focusView();
	SCUM_View* focus = prevFocus(true, foundFocus);
	if (!focus && foundFocus) focus = prevFocus(true, foundFocus);
	if (focus) focus->makeFocus(true, true);
}

// EOF
