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

	$Id: SCUM_FLSystem.cpp,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#include "SCUM_FLApp.hh"
#include "SCUM_Font.hh"
#include "SCUM_GL.hh"
#include "SCUM_Image.hh"
#include "SCUM_Menu.hh"
#include "SCUM_NSKey.h"
#include "SCUM_System.hh"
#include "SCUM_Window.hh"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_draw.H>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <vector>

// =====================================================================
// SCUM_FLFont

class SCUM_FLFont : public SCUM_FontHandle
{
public:
	SCUM_FLFont(const char* name, int size);

	virtual float height();
	virtual float ascent();
	virtual float descent();

	virtual SCUM_Size measure(const char* str);

	inline void saveFont(int& id, int& size);
	inline void restoreFont(int id, int size);

	int			m_id;
	int			m_size;
};

// =====================================================================
// SCUM_System

namespace SCUM
{
	struct GCState
	{
		void init();
		inline void apply();

		Fl_Color		color;
		int				lineWidth;
		int				lineStyle;
	};

	static GCState gGCState;
	static std::vector<GCState> gGCStateStack;
};

// =====================================================================
// SCUM_FLWindow

class SCUM_FLWindow : public Fl_Double_Window, public SCUM_WindowHandle
{
public:
	SCUM_FLWindow(SCUM_Window* view);
	virtual ~SCUM_FLWindow();

	SCUM_Rect bounds();

	// return state flags
	virtual bool isFullscreen();
	virtual bool isDecorated();
	virtual bool isModal();

	virtual void setTitle(const char* str);
	virtual void setPos(const SCUM_Point& pos);
	virtual void setSize(const SCUM_Size& size);
	virtual void setMinSize(const SCUM_Size& size);
	virtual void setMaxSize(const SCUM_Size& size);
	virtual void setFullscreen(bool flag);
	virtual void setDecorated(bool flag);
	virtual void setModal(bool flag);

	virtual void destroy();
	virtual void show();
	virtual void hide();

	virtual void refresh(const SCUM_Rect& damage);

	// overrides
	virtual int handle(int evt);
	virtual void resize(int x, int y, int w, int h);
	virtual void draw();

	// util
	SCUM_Point makePoint(int x, int y)
	{
 		return SCUM_Point(x, h() - y);
	}

	static wint_t eventKey(int& state);
	static SCUM_Point eventPos()
	{
		return SCUM_Point(Fl::event_x(), Fl::event_y());
	}

private:
	static void closeCB(Fl_Widget* w, void*);

private:
	std::string			m_title;
	SCUM_Size			m_minSize;
	SCUM_Size			m_maxSize;
	bool				m_fullscreen;
	SCUM_Rect			m_saveBounds;
};

// =====================================================================
// SCUM_FLGLContext

class SCUM_FLGLContext : public Fl_Gl_Window, public SCUM_GLContext
{
public:
	SCUM_FLGLContext(Fl_Group* parent, SCUM_View* view);
	virtual ~SCUM_FLGLContext();

	virtual SCUM_Rect bounds();
	virtual void setBounds(const SCUM_Rect& bounds);
	virtual void refresh();

	virtual int handle(int);
	virtual void draw();
};

// =====================================================================
// SCUM_FLMenu

class SCUM_FLMenu : public SCUM_MenuHandle
{
	struct Item
	{
		std::string		text;
		int				index;
	};

public:
	SCUM_FLMenu(const SCUM_MenuItems& items);
	virtual ~SCUM_FLMenu();

	virtual bool value(int item) const;
	virtual void setValue(int item, bool value);
	virtual int popup(const SCUM_Point& where, int item);

private:
	size_t				m_size;
	Item*				m_items;
	Fl_Menu_Item*		m_data;
};

// =====================================================================
// SCUM_FLImage

class SCUM_FLImage : public SCUM_ImageHandle
{
public:
	SCUM_FLImage(const char* fileName, const SCUM_Size& size);
	virtual ~SCUM_FLImage();

	Fl_Shared_Image* image() { return m_image; }

	virtual SCUM_Size size();
	virtual void draw(const SCUM_Point& p);

private:
	Fl_Shared_Image*	m_image;
};

// =====================================================================
// Util

namespace SCUM
{
// 	inline SCUM_Color convert(const Fl_Color& color)
// 	{
// 		const uchar* c = (uchar*)&color;
//   if (i & 0xffffff00) {
//     unsigned rgb = (unsigned)i;
//     fl_color((uchar)(rgb >> 24), (uchar)(rgb >> 16), (uchar)(rgb >> 8));
//   } else {

// 		return SCUM_Color((c
// 	}

	int getFontId(const char* name)
	{
		int id = FL_HELVETICA;
		
		if (strstr(name, "Helvetica") == name) {
			id = FL_HELVETICA;
		} else if (strstr(name, "Times") == name) {
			id = FL_TIMES;
		} else if (strstr(name, "Courier") == name) {
			id = FL_COURIER;
		}
		
		name = index(name, '-');
		while (name) {
			if (strstr(name, "-Bold") == name) {
				id |= FL_BOLD;
			} else if (strstr(name, "-Italic") == name) {
				id |= FL_ITALIC;
			}
			name = index(++name, '-');
		}
		
		return id;
	}
};

// =====================================================================
// SCUM_FLFont

SCUM_FontHandle* SCUM::makeFontHandle(const char* name, float size)
{
	return new SCUM_FLFont(name, (int)size);
}

SCUM_FLFont::SCUM_FLFont(const char* name, int size)
	: m_size(size)
{
	m_id = SCUM::getFontId(name);
}

inline void SCUM_FLFont::restoreFont(int id, int size)
{
	fl_font(id, size);
}

inline void SCUM_FLFont::saveFont(int& id, int& size)
{
	id = fl_font();
	size = fl_size();
	restoreFont(m_id, m_size);
}

float SCUM_FLFont::height()
{
	int id, size;
	float res;

	saveFont(id, size);
	res = (float)fl_height();
	restoreFont(id, size);

	return res;
}

float SCUM_FLFont::ascent()
{
	int id, size;
	float res;

	saveFont(id, size);
	res = (float)(fl_height() - fl_descent());
	restoreFont(id, size);

	return res;
}

float SCUM_FLFont::descent()
{
	int id, size;
	float res;

	saveFont(id, size);
	res = (float)fl_descent();
	restoreFont(id, size);

	return res;
}

SCUM_Size SCUM_FLFont::measure(const char* str)
{
	int id, size;
	int w=0, h=0;
	
	saveFont(id, size);
	fl_measure(str, w, h, 0);
	restoreFont(id, size);

	return SCUM_Size(w, h);
}

// =====================================================================
// SCUM_System

#include <time.h>
#include <sys/time.h>

void SCUM::GCState::init()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);
	fl_font(FL_HELVETICA, 12);
}

inline void SCUM::GCState::apply()
{
	fl_color(gGCState.color);
	fl_line_style(gGCState.lineStyle, gGCState.lineWidth);
}

void SCUM::init()
{
	gGCState.init();
}

// void SCUM::quit(int returnCode)
// {
// 	SCUM_FLApp::instance()->quit(returnCode);
// }

SCUM_Size SCUM::screenSize()
{
	return SCUM_Size();
}

double SCUM::time()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;
}

namespace SCUM
{
	static void timerCB(void* obj)
	{
		SCUM_Timer* timer = (SCUM_Timer*)obj;
		if (timer->isValid()) {
			(*timer)();
			Fl::repeat_timeout(timer->timeout(), &timerCB, timer);
		} else {
			Fl::remove_timeout(&timerCB, timer);
			delete timer;
		}
	}
};

void SCUM::addTimer(SCUM_Timer* timer)
{
	Fl::add_timeout(timer->timeout(), &timerCB, timer);
}

void SCUM::pushClip(float x, float y, float w, float h)
{
	fl_push_clip((int)x, (int)y, (int)w, (int)h);
}

void SCUM::pushNoClip()
{
	fl_push_no_clip();
}

void SCUM::popClip()
{
	fl_pop_clip();
}

bool SCUM::isClipped(const SCUM_Rect& r)
{
	return !fl_not_clipped((int)r.x, (int)r.y, (int)r.w, (int)r.h);
}

SCUM_Rect SCUM::clippedRect(const SCUM_Rect& r)
{
	int x, y, w, h;
	if (fl_clip_box((int)r.x, (int)r.y, (int)r.w, (int)r.h, x, y, w, h))
		return SCUM_Rect(x, y, w, h);
	return r;
}

void SCUM::saveGCState()
{
	gGCStateStack.push_back(gGCState);
}

void SCUM::restoreGCState()
{
	gGCState = gGCStateStack.back();
	gGCState.apply();
	gGCStateStack.pop_back();
}

void SCUM::setFont(SCUM_FontHandle* fontHandle)
{
	assert(fontHandle != 0);
	SCUM_FLFont* font = dynamic_cast<SCUM_FLFont*>(fontHandle);
	assert(font != 0);
	fl_font(font->m_id, font->m_size);
}

void SCUM::setColor(const SCUM_Color& c)
{
	gGCState.color = fl_rgb_color(c.r8(), c.g8(), c.b8());
	fl_color(gGCState.color);
}

void SCUM::setLineStyle(float w, int style)
{
	switch (style) {
		case SCUM::LineSolid: style = FL_SOLID; break;
		case SCUM::LineDash: style = FL_DASH; break;
		case SCUM::LineDot: style = FL_DOT; break;
	}
	gGCState.lineWidth = (int)w;
	gGCState.lineStyle = style;
	fl_line_style(gGCState.lineStyle, gGCState.lineWidth);
}

void SCUM::drawPoint(float x, float y)
{
	fl_point((int)x, (int)y);
}

void SCUM::drawLine(float x1, float y1, float x2, float y2)
{
	fl_line((int)x1, (int)y1, (int)x2, (int)y2);
}

void SCUM::drawRect(float x, float y, float w, float h)
{
	fl_rect((int)x, (int)y, (int)w, (int)h);
}

namespace SCUM
{
	inline void drawBeveledRectNW(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_yxline(x1++, y2--, y1++, x2--);
	}

	inline void drawBeveledRectSE(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_xyline(x1++, y2--, x2--, y1++);
	}
};

void SCUM::drawBeveledRect(float xf, float yf, float wf, float hf, float bwf, bool inout)
{
	Fl_Color bc = gGCState.color;
	SCUM_Color sbc = SCUM_Color((uint32_t)bc & 0xFFFFFF00);
	Fl_Color lc = (Fl_Color)(sbc.blend(SCUM_Color(1, 1, 1), 0.3).rgba32() & 0xFFFFFF00);
	Fl_Color dc = (Fl_Color)(sbc.blend(SCUM_Color(0, 0, 0), 0.3).rgba32() & 0xFFFFFF00);

	int x1 = (int)xf;
	int y1 = (int)yf;
	int x2 = (int)(xf+wf) - 1;
	int y2 = (int)(yf+hf) - 1;
	int bw = (int)bwf;

	// draw SE
	if (inout) fl_color(lc);
	else fl_color(dc);
	drawBeveledRectSE(x1, y1, x2, y2, bw);

	// draw NW (overlaps SE)
	if (inout) fl_color(dc);
	else fl_color(lc);
	drawBeveledRectNW(x1, y1, x2, y2, bw);

	// restore color
	fl_color(bc);
}

void SCUM::fillRect(float x, float y, float w, float h)
{
	fl_rectf((int)x, (int)y, (int)w, (int)h);
}

void SCUM::fillRoundedRect(float x, float y, float w, float h, float r)
{
	r = 10;
	float r2 = 2.0f * r;
	
	float x1 = x + r2;
	float y1 = y + r2;
	float x2 = x + w - r2;
	float y2 = y + h - r2;

	float w1 = w - r2;
	float h1 = h - r2;

	fl_pie((int)x, (int)y, (int)r2-1, (int)r2-1, 90, 180); // NW
	fl_pie((int)x2, (int)y, (int)r2, (int)r2, 0, 90); // NE
	fl_pie((int)x, (int)y2, (int)r2, (int)r2, 180, 270); // SW
	fl_pie((int)x2, (int)y2, (int)r2, (int)r2, 0, -90); // SE

	fl_rectf((int)(x+r), (int)(y+r), (int)w1, (int)h1);
	fl_rectf((int)(x+r), (int)y, (int)w1, (int)r); // N
	fl_rectf((int)(x+w-r), (int)(y+r), (int)r, (int)h1); // E
	fl_rectf((int)(x+r), (int)(y+h-r), (int)w1, (int)r); // S
	fl_rectf((int)x, (int)(y+r), (int)r, (int)h1); // W
}

void SCUM::drawPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_loop((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::fillPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_polygon((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::drawArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_arc((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::fillArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_pie((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::drawText(float x, float y, const char* str)
{
	fl_draw(str, (int)x, (int)y);
}

// =====================================================================
// SCUM_FLWindow

SCUM_WindowHandle* SCUM::makeWindowHandle(SCUM_Window* view)
{
	return new SCUM_FLWindow(view);
}

wint_t SCUM_FLWindow::eventKey(int& state)
{
	const int key = Fl::event_key();

	//printf("0x%x\n", key);

	// printable ascii
	if ((key > 0) && (key < 256)) {
		int c = Fl::event_text()[0];
		if (isprint(c) && !iscntrl(c)) {
			return c;
		} else {
			return key;
		}
	}

	// keypad
	if ((key > FL_KP) && (key <= FL_KP_Last)) {
		state |= SCUM::ModKeypad;
		return key - FL_KP;
	}

	// function keys
	if ((key > FL_F) && (key <= FL_F_Last)) {
		return NSF1FunctionKey + (key - FL_F) - 1;
	}

	switch (key) {
		case FL_Escape:
			return 0x1b;
		case FL_BackSpace:
			return 0x08;
		case FL_Tab:
			return 0x09;
		case 0xfe20: // backtab
			state |= SCUM::ModShift;
			return 0x09;
		case FL_Enter:
			return 0x0a;
		case FL_Print:
			return NSPrintFunctionKey;
		case FL_Scroll_Lock:
			return NSScrollLockFunctionKey;
		case FL_Pause:
			return NSPauseFunctionKey;
		case FL_Insert:
			return NSInsertFunctionKey;
		case FL_Home:
			return NSHomeFunctionKey;
		case FL_Page_Up:
			return NSPageUpFunctionKey;
		case FL_Delete:
			return NSDeleteFunctionKey;
		case FL_End:
			return NSEndFunctionKey;
		case FL_Page_Down:
			return NSPageDownFunctionKey;
		case FL_Left:
			return NSLeftArrowFunctionKey;
		case FL_Up:
			return NSUpArrowFunctionKey;
		case FL_Right:
			return NSRightArrowFunctionKey;
		case FL_Down:
			return NSDownArrowFunctionKey;
		case FL_Shift_L:
		case FL_Shift_R:
			state |= SCUM::ModShift;
			return 0;
		case FL_Control_L:
		case FL_Control_R:
			state |= SCUM::ModControl;
			return 0;
		case FL_Caps_Lock:
			return 0;
		case FL_Alt_L:
		case FL_Alt_R:
		case FL_Meta_L:
		case FL_Meta_R:
			state |= SCUM::ModCommand;
			return 0;
		case FL_Menu: return NSMenuFunctionKey;
		case FL_Num_Lock:
			return 0;
		case FL_KP_Enter:
			state |= SCUM::ModKeypad;
			return 0x0a;
	}

	return 0;
}

SCUM_FLWindow::SCUM_FLWindow(SCUM_Window* view)
	: Fl_Double_Window(0, 0, 0, 0),
	  SCUM_WindowHandle(view),
	  m_fullscreen(0)
{
	end();
	callback(closeCB, view);
	free_position();
}

SCUM_FLWindow::~SCUM_FLWindow()
{
}

SCUM_Rect SCUM_FLWindow::bounds()
{
	return SCUM_Rect(x(), y(), w(), h());
}

bool SCUM_FLWindow::isFullscreen()
{
	return m_fullscreen;
}

bool SCUM_FLWindow::isDecorated()
{
	return border();
}

bool SCUM_FLWindow::isModal()
{
	return modal();
}

void SCUM_FLWindow::setTitle(const char* str)
{
	m_title = str;
	label(m_title.c_str());
}

void SCUM_FLWindow::setPos(const SCUM_Point& pos)
{
}

void SCUM_FLWindow::setSize(const SCUM_Size& size)
{
	Fl_Double_Window::size((int)size.w, (int)size.h);
}

void SCUM_FLWindow::setMinSize(const SCUM_Size& size)
{
	m_minSize = size;
	size_range(
		(int)size.w, (int)size.h,
		(int)m_maxSize.w, (int)m_maxSize.h
		);
}

void SCUM_FLWindow::setMaxSize(const SCUM_Size& size)
{
	m_maxSize = size;
	size_range(
		(int)m_minSize.w, (int)m_minSize.h,
		(int)size.w, (int)size.h
		);
}

void SCUM_FLWindow::setFullscreen(bool flag)
{
	if (flag != m_fullscreen) {
		m_fullscreen = flag;
		if (flag) {
			m_saveBounds = bounds();
			fullscreen();
		} else {
			fullscreen_off(
				(int)m_saveBounds.x, (int)m_saveBounds.y,
				(int)m_saveBounds.w, (int)m_saveBounds.h
				);
		}
	}
}

void SCUM_FLWindow::setDecorated(bool flag)
{
	border(flag);
}

void SCUM_FLWindow::setModal(bool flag)
{
	if (flag) set_modal();
	else set_non_modal();
}

void SCUM_FLWindow::destroy()
{
	// IMPORTANT: since some virtual SCUM_WindowHandle methods are
	// called from view destructors, _first_ the view has to be
	// deleted, _then_ the handle. here is where it happens.
	destroyView();
	delete this;
}

void SCUM_FLWindow::show()
{
	Fl_Double_Window::show();
}

void SCUM_FLWindow::hide()
{
	Fl_Double_Window::hide();
}

void SCUM_FLWindow::refresh(const SCUM_Rect& damage)
{
	Fl_Double_Window::damage(
		FL_DAMAGE_ALL,
		(int)damage.x, (int)damage.y,
		(int)damage.w, (int)damage.h
		);
}

int SCUM_FLWindow::handle(int evt)
{
	int flstate = Fl::event_state();
	int state = 0;
	wint_t key;

	if (flstate & FL_SHIFT) state |= SCUM::ModShift;
	if (flstate & FL_CTRL) state |= SCUM::ModControl;
	if ((flstate & FL_ALT) || (flstate & FL_META)) state |= SCUM::ModCommand;

	switch (evt) {
		case FL_PUSH:
			if (Fl::event_button() == 3) {
				view()->contextMenu(state, eventPos());
			} else {
				view()->mouseDown(state, eventPos());
			}
			return true;
		case FL_DRAG:
			view()->mouseMove(state, eventPos());
			return true;
		case FL_RELEASE:
			view()->mouseUp(state, eventPos());
			return true;
		case FL_MOUSEWHEEL:
		{
			float dx = Fl::event_dx();
			float dy = Fl::event_dy();
			if (state & SCUM::ModControl) {
				// swap axes
				float tmp = dx;
				dx = dy;
				dy = tmp;
				// strip modifier
				state &= ~SCUM::ModControl;
			}
			view()->scrollWheel(state, eventPos(), SCUM_Point(dx, dy));
		}
		return true;
		case FL_FOCUS:
			view()->focusEvent(true);
			return true;
		case FL_UNFOCUS:
			view()->focusEvent(false);
			return true;
		case FL_KEYDOWN:
			key = eventKey(state);
			view()->keyDown(state, key);
			return true;
		case FL_KEYUP:
			key =  eventKey(state);
			view()->keyUp(state, key);
			return true;
		case FL_SHOW:
			view()->showEvent();
			return Fl_Double_Window::handle(evt);
		case FL_HIDE:
			view()->hideEvent();
			return true;
	}

	return Fl_Double_Window::handle(evt);
}

void SCUM_FLWindow::resize(int x, int y, int w, int h)
{
	Fl_Double_Window::resize(x, y, w, h);
	view()->resizeEvent(SCUM_Rect(x, y, w, h));
}

void SCUM_FLWindow::draw()
{
	view()->draw();
}

void SCUM_FLWindow::closeCB(Fl_Widget* w, void* arg)
{
	((SCUM_Window*)arg)->closeEvent();
}

// =====================================================================
// SCUM_FLGLContext

SCUM_GLContext* SCUM::makeGLContext(SCUM_WindowHandle* window, SCUM_View* view)
{
	return new SCUM_FLGLContext(dynamic_cast<Fl_Group*>(window), view);
}

SCUM_GLContext* SCUM::makeGLContext(SCUM_View* view)
{
	return SCUM::makeGLContext(view->window()->handle(), view);
}

SCUM_FLGLContext::SCUM_FLGLContext(Fl_Group* parent, SCUM_View* view)
	: Fl_Gl_Window(0, 0, 0, 0),
	  SCUM_GLContext(view)
{
	mode(FL_RGB|FL_DOUBLE|FL_DEPTH);
	invalidate();
	resize(0, 0, 0, 0);
	parent->add(this);
}

SCUM_FLGLContext::~SCUM_FLGLContext()
{
	parent()->remove(this);
}

SCUM_Rect SCUM_FLGLContext::bounds()
{
	return SCUM_Rect(x(), y(), w(), h());
}

void SCUM_FLGLContext::setBounds(const SCUM_Rect& bounds)
{
	resize((int)bounds.x, (int)bounds.y, (int)bounds.w, (int)bounds.h);
}

void SCUM_FLGLContext::refresh()
{
	redraw();
}

int SCUM_FLGLContext::handle(int evt)
{
	if (evt == FL_SHOW)
		return Fl_Gl_Window::handle(evt);
	return 0;
}

void SCUM_FLGLContext::draw()
{
	if (!valid()) {
		view()->initGL();
		view()->resizeGL(w(), h());
	}
	view()->drawGL();
}

// =====================================================================
// SCUM_FLMenu

SCUM_MenuHandle* SCUM::makeMenuHandle(const SCUM_MenuItems& items)
{
	return new SCUM_FLMenu(items);
}

SCUM_FLMenu::SCUM_FLMenu(const SCUM_MenuItems& items)
{
	m_size = items.size();
	m_items = new Item[m_size];
	m_data = new Fl_Menu_Item[m_size+1];

	int* lastFlags = 0;

	for (size_t i=0; i < m_size; i++) {
		Item* item = m_items + i;
		Fl_Menu_Item* data = m_data + i;

		item->text = items[i].text();
		item->index = i;

		memset(data, 0, sizeof(Fl_Menu_Item));

		data->text = item->text.c_str();
		data->user_data(item);
		data->labeltype(FL_NORMAL_LABEL);
		data->labelcolor(FL_BLACK);
		data->labelfont(FL_HELVETICA);
		data->labelsize(12);

		switch (items[i].type()) {
			case SCUM::MenuSeparator:
				data->flags = FL_MENU_INVISIBLE;
				if (lastFlags) *lastFlags |= FL_MENU_DIVIDER;
				break;
			case SCUM::MenuToggle:
				data->flags = FL_MENU_TOGGLE;
				break;
			case SCUM::MenuRadio:
				data->flags = FL_MENU_RADIO;
				break;
			case SCUM::MenuOpen:
				data->flags = FL_SUBMENU;
				break;
			case SCUM::MenuClose:
				data->text = 0;
		}

		lastFlags = &data->flags;
	}

	m_data[m_size].text = 0;
}

SCUM_FLMenu::~SCUM_FLMenu()
{
	delete [] m_items;
	delete [] m_data;
}

bool SCUM_FLMenu::value(int item) const
{
	return (item >= 0) && (item < m_size) && m_data[item].value();
}

void SCUM_FLMenu::setValue(int item, bool value)
{
	if ((item >= 0) && (item < m_size)) {
		Fl_Menu_Item* data = m_data + item;
		if (data->flags & FL_MENU_TOGGLE) {
			if (value) data->set();
			else data->clear();
		} else if (data->flags & FL_MENU_RADIO) {
			if (value) data->setonly();
			else data->clear();
		}
	}
}

int SCUM_FLMenu::popup(const SCUM_Point& where, int item)
{
	const Fl_Menu_Item* data
		= (item >= 0) && (item < m_size) ? m_data + item : 0;

	const Fl_Menu_Item* res
		= m_data->popup((int)where.x, (int)where.y, 0, data);

	return res ? ((Item*)res->user_data())->index : -1;
}

// =====================================================================
// SCUM_FLImage

SCUM_ImageHandle* SCUM::makeImageHandle(const char* fileName, const SCUM_Size& size)
{
	return new SCUM_FLImage(fileName, size);
}

SCUM_FLImage::SCUM_FLImage(const char* fileName, const SCUM_Size& size)
{
	int w = (int)size.w;
	int h = (int)size.h;
	m_image = Fl_Shared_Image::get(fileName, w, h);
}

SCUM_FLImage::~SCUM_FLImage()
{
	if (m_image) m_image->release();
}

SCUM_Size SCUM_FLImage::size()
{
	if (m_image) return SCUM_Size(m_image->w(), m_image->h());
	return SCUM_Size();
}

void SCUM_FLImage::draw(const SCUM_Point& p)
{
	if (m_image) m_image->draw((int)p.x, (int)p.y);
}

// EOF
