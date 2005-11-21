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


#include "SCUM_Window.hh"
#include "SCUM_Client.hh"
#include "SCUM_FLUtil.hh"
#include "SCUM_System.hh"
#include "SCUM_Util.hh"

#include <iostream>
#include <FL/fl_draw.H>

using namespace SCUM;

// =====================================================================
// SCUM_Window

SCUM_Window::SCUM_Window(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Bin(klass, client, oid, args),
      Fl_Double_Window(0, 0),
      m_deferredTimer(0),
      m_mouseMoveView(0),
      m_focusView(0),
      m_saveFocusView(0)
{
    end();
    callback(closeCB, this);
    free_position();

    m_window = this;

    SCUM_View::flags().vVisible = false;
    SCUM_View::flags().wResizable = true;
    SCUM_View::flags().wFullscreen = false;
    SCUM_View::flags().wShowFocus = false;

    memset(&m_deferredCommands, 0, sizeof(DeferredCommands));

    m_deferredTimer
	= new SCUM_MemTimer<SCUM_Window>(0.075, &SCUM_Window::deferredAction, this);
    addTimer(m_deferredTimer);

    getClient()->retain(this);
}

SCUM_Window::~SCUM_Window()
{
    getClient()->release(this);
    if (m_deferredTimer) {
	m_deferredTimer->remove();
	m_deferredTimer = 0;
    }
    // this is done in the language from closeEvent or destroy
    // sendMessage(SCUM_Symbol::prDestroyed, 0, 0);
}

void SCUM_Window::refresh(const SCUM_Rect& damage)
{
    //	std::cout << "SCUM_Window::refresh " << damage << "\n";
    this->damage(
		 FL_DAMAGE_CHILD,
		 damage.xi(), damage.yi(),
		 damage.wi(), damage.hi()
		 );
}

void SCUM_Window::refresh()
{
    refresh(bounds());
}

void SCUM_Window::draw(const SCUM_Rect& damage)
{
    //	std::cout << "SCUM_Window::draw: " << damage << " " << isVisible() << "\n";
    SCUM_Bin::draw(damage);
    if (m_focusView && SCUM_View::flags().wShowFocus) m_focusView->drawFocus(damage);
}

void SCUM_Window::raise()
{
    show();
}

void SCUM_Window::lower()
{
}

void SCUM_Window::closeEvent()
{
    OSC::StaticClientPacket<24> p;
    p.openMessage("/event", 2);
    putId(p);
    p.putString("close");
    p.closeMessage();
    send(p);
}

void SCUM_Window::showEvent()
{
    SCUM_View::flags().vVisible = true;
}

void SCUM_Window::hideEvent()
{
    SCUM_View::flags().vVisible = false;
}

void SCUM_Window::resizeEvent(const SCUM_Rect& bounds)
{
    m_screenBounds = bounds;
    setBounds(SCUM_Rect(0.f, 0.f, bounds.w, bounds.h));
}

void SCUM_Window::focusEvent(bool hasFocus)
{
    SCUM_View* refreshView = 0;

    if (hasFocus) {
	m_focusView = m_saveFocusView;
	m_saveFocusView = 0;
	refreshView = m_focusView;
    } else {
	m_saveFocusView = m_focusView;
	m_focusView = 0;
	refreshView = m_saveFocusView;
    }

    if (refreshView) refreshView->refreshFocus();
}

void SCUM_Window::setMouseView(SCUM_View* view)
{
    //SCUM_ASSERT(m_mouseMoveView == 0); ??
    m_mouseMoveView = view;
}

void SCUM_Window::mouseMove(int state, const SCUM_Point& where)
{
    if (m_mouseMoveView) {
	m_mouseMoveView->mouseMove(state, where);
    }
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
	} else if (state == kModMaskShift) {
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
    layout().changed = true;
    m_deferredCommands.updateLayout = true;
    m_deferredCommands.xFit = m_deferredCommands.xFit || xFit;
    m_deferredCommands.yFit = m_deferredCommands.yFit || yFit;
}

SCUM_Size SCUM_Window::getMinSize()
{
    return SCUM_Bin::getMinSize().max(10);
}

void SCUM_Window::doUpdateLayout(bool xFit, bool yFit)
{
    SCUM_Size newSize = minSize();
    SCUM_Size curSize = bounds().size();
    SCUM_Size minSize = newSize;

    if (!shown()) {
	newSize = newSize.max(m_initialSize);
    } else {
	if (!xFit) newSize.w = max(newSize.w, curSize.w);
	if (!yFit) newSize.h = max(newSize.h, curSize.h);
    }

    //	std::cout << "curSize " << curSize << " newSize " << newSize << "\n";

    // release constraints
    size_range(0, 0);

    // set size
    if (newSize == curSize) {
	setBounds(bounds());
    } else {
	Fl_Double_Window::size(newSize.wi(), newSize.hi());
    }

    // enforce constraints
    if (SCUM_View::flags().wResizable) {
	size_range(minSize.wi(), minSize.hi());
    } else {
	int w = newSize.wi();
	int h = newSize.hi();
	size_range(w, h, w, h);
    }
}

void SCUM_Window::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "visible")) { // overridden from SCUM_View
	if (args.get_i()) {
	    m_deferredCommands.show = true;
	    m_deferredCommands.hide = false;
	} else {
	    m_deferredCommands.show = false;
	    m_deferredCommands.hide = true;
	}
    } else if (equal(key, "title")) {
	m_title = args.get_s();
	label(m_title.c_str());
    } else if (equal(key, "initialSize")) {
	m_initialSize = SCUM_Size(args.get_f(), args.get_f());
    } else if (equal(key, "resizable")) {
	SCUM_View::flags().wResizable = args.get_i();
	updateLayout(false, false);
    } else if (equal(key, "fullscreen")) {
	//m_deferredCommands.fullscreen = args.get_i();
    } else if (equal(key, "decorated")) {
	bool flag = args.get_i();
	if (flag != border()) {
	    border(flag);
	}
    } else if (equal(key, "modal")) {
	bool flag = args.get_i();
	if (flag != modal()) {
	    if (flag) set_modal();
	    else set_non_modal();
	}
    } else if (equal(key, "showFocus")) {
	bool flag = args.get_i();
	if (flag != SCUM_View::flags().wShowFocus) {
	    SCUM_View::flags().wShowFocus = flag;
	    if (m_focusView) m_focusView->refreshFocus();
	}
    } else {
	SCUM_Bin::setProperty(key, args);
    }
}

#if 0
void SCUM_Window::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (equal(key, "initialSize")) {
	setSizeValue(slot, m_initialSize);
    } else if (equal(key, "screenBounds")) {
	setRectValue(slot, m_screenBounds);
    } else if (equal(key, "resizable")) {
	setBoolValue(slot, SCUM_View::flags().wResizable);
    } else if (equal(key, "fullscreen")) {
	setBoolValue(slot, SCUM_View::flags().wFullscreen);
    } else if (equal(key, "decorated")) {
	setBoolValue(slot, border());
    } else if (equal(key, "modal")) {
	setBoolValue(slot, modal());
    } else if (equal(key, "showFocus")) {
	setBoolValue(slot, SCUM_View::flags().wShowFocus);
    } else {
	SCUM_Bin::getProperty(key, slot);
    }
}
#endif

void SCUM_Window::deferredAction(SCUM_Timer*)
{
#ifdef LATER
    if (handle()->flags().fullscreen != m_deferredCommands.fullscreen) {
	WindowHandle::Flags flags(handle()->flags());
	flags.fullscreen = m_deferredCommands.fullscreen;
	handle()->setFlags(flags);
	m_deferredCommands.fullscreen = handle()->flags().fullscreen;
    }
#endif

    if (m_deferredCommands.show) {
	m_deferredCommands.show = false;
	if (!shown()) {
	    doUpdateLayout(false, false);
	    m_deferredCommands.updateLayout = false;
	}
	show();
    } else if (m_deferredCommands.hide) {
	m_deferredCommands.hide = false;
	hide();
    }

    // currently unused
    // user resizes are propagated instantly
#if 0
    if (m_deferredCommands.resize) {
	m_deferredCommands.resize = false;
	SCUM_Rect newBounds(m_screenBounds);
	newBounds.x = newBounds.y = 0.0f;
	//		if (newBounds != bounds()) {
	setBounds(newBounds);
	//		}
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

int SCUM_Window::handle(int evt)
{
    switch (evt) {
    case FL_PUSH:
	if (Fl::event_button() == 3) {
	    contextMenu(SCUM::FLEventState(), SCUM::FLEventPos());
	} else {
	    mouseDown(SCUM::FLEventState(), SCUM::FLEventPos());
	}
	return true;
    case FL_DRAG:
	mouseMove(SCUM::FLEventState(), SCUM::FLEventPos());
	return true;
    case FL_RELEASE:
	mouseUp(SCUM::FLEventState(), SCUM::FLEventPos());
	return true;
    case FL_MOUSEWHEEL:
	{
	    float dx = Fl::event_dx();
	    float dy = Fl::event_dy();
	    int state = SCUM::FLEventState();
	    if (state & SCUM::kModMaskControl) {
		// swap axes
		float tmp = dx;
		dx = dy;
		dy = tmp;
		// strip modifier
		state &= ~SCUM::kModMaskControl;
	    }
	    scrollWheel(state, SCUM::FLEventPos(), SCUM_Point(dx, dy));
	}
	return true;
    case FL_FOCUS:
	focusEvent(true);
	return true;
    case FL_UNFOCUS:
	focusEvent(false);
	return true;
    case FL_KEYDOWN:
	{
	    int state = SCUM::FLEventState();
	    wint_t key = SCUM::FLEventKey(state);
	    keyDown(state, key);
	}
	return true;
    case FL_KEYUP:
	{
	    int state = SCUM::FLEventState();
	    wint_t key = SCUM::FLEventKey(state);
	    keyUp(state, key);
	}
	return true;
    case FL_SHOW:
	showEvent();
	break;
    case FL_HIDE:
	hideEvent();
	break;
    }

    return Fl_Double_Window::handle(evt);
}

void SCUM_Window::resize(int x, int y, int w, int h)
{
    Fl_Double_Window::resize(x, y, w, h);
    // this is necessary on OSX:
    SCUM_Size ms(minSize().max(SCUM_Size(w, h)));
    m_screenBounds = SCUM_Rect(x, y, ms.w, ms.h);
    setBounds(SCUM_Rect(0.f, 0.f, ms.w, ms.h));
}

void SCUM_Window::draw()
{
    int X, Y, W, H;
    if (!fl_clip_box(0, 0, w(), h(), X, Y, W, H)) {
	X = Y = 0;
	W = w();
	H = h();
    }
    draw(SCUM_Rect(X, Y, W, H));
}

void SCUM_Window::closeCB(Fl_Widget* w, void* arg)
{
    ((SCUM_Window*)arg)->closeEvent();
}

#include "SCUM_Class.hh"

void SCUM_Window_Init(SCUM_ClassRegistry* reg)
{
    new SCUM_ClassT<SCUM_Window>(reg, "Window", "Bin");
}

// EOF
