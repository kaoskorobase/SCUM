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

	$Id: SCUM_FLSystem.cpp,v 1.2 2004/08/04 11:48:25 steve Exp $
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
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_draw.H>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace SCUM
{
	// =====================================================================
	// SCUM::FL::FontHandle

	class FLFontHandle : public FontHandle
	{
	public:
		FLFontHandle(const char* name, float size);

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
	// SCUM_FLWindow

#	define SCUM_FL_WINDOW_HANDLE_SUPER Fl_Single_Window

	class FLWindowHandle : protected SCUM_FL_WINDOW_HANDLE_SUPER, public WindowHandle
	{
	public:
		FLWindowHandle(SCUM_Window* view);
		virtual ~FLWindowHandle();
		
		// interface
		virtual SCUM_Rect bounds();
		
		virtual Flags flags();
		virtual void setFlags(const Flags& flags);

		virtual void setTitle(const char* str);
		virtual void setPos(const SCUM_Point& pos);
		virtual void setSize(const SCUM_Size& size);
		virtual void setMinSize(const SCUM_Size& size);
		virtual void setMaxSize(const SCUM_Size& size);
		
		virtual void show();
		virtual void hide();
		
		virtual void refresh(const SCUM_Rect& damage);

	protected:
		// overrides
		virtual int handle(int evt);
		virtual void resize(int x, int y, int w, int h);
		virtual void draw();

	private:
		static void closeCB(Fl_Widget* w, void*);

	private:
		SCUM_Window*	m_view;
		Flags			m_flags;
		std::string		m_title;
		SCUM_Size		m_minSize;
		SCUM_Size		m_maxSize;
		SCUM_Rect		m_saveBounds;
		SCUM_Rect		m_damage;
	};

	// =====================================================================
	// SCUM_FLMenu

	class FLMenuHandle : public MenuHandle
	{
		struct Item
		{
			std::string		text;
			int				index;
		};
		
	public:
		FLMenuHandle(const std::vector<SCUM_MenuItem>& items);
		virtual ~FLMenuHandle();
		
		virtual bool value(int item);
		virtual void setValue(int item, bool value);
		virtual int popup(const SCUM_Point& where, int item);
		
	private:
		size_t				m_size;
		Item*				m_items;
		Fl_Menu_Item*		m_data;
	};

	// =====================================================================
	// SCUM_FLImage

	class FLImageHandle : public ImageHandle
	{
	public:
		FLImageHandle(const char* fileName, const SCUM_Size& size);
		virtual ~FLImageHandle();
		
		Fl_Shared_Image* image() { return m_image; }
		
		virtual SCUM_Size size();
		
	private:
		Fl_Shared_Image*	m_image;
	};

	// =====================================================================
	// SCUM_FLGLContext

	class FLGLContext : protected Fl_Gl_Window, public GLContext
	{
	public:
		FLGLContext(Fl_Group* parent, SCUM_View* view);
		virtual ~FLGLContext();
		
		virtual SCUM_Rect bounds();
		virtual void setBounds(const SCUM_Rect& bounds);
		virtual void refresh();
		
	protected:
		virtual int handle(int);
		virtual void draw();

	private:
		SCUM_View*		m_view;
	};

	// =====================================================================
	// SCUM_System
	
	static int gFL_clipStackDepth = 0;

	inline void FL_pushClip(const SCUM_Rect& r)
	{
		gFL_clipStackDepth++;
		fl_push_clip((int)r.x, (int)r.y, (int)r.w, (int)r.h);
	}
	inline void FL_pushNoClip()
	{
		gFL_clipStackDepth++;
		fl_push_no_clip();
	}
	inline void FL_popClip()
	{
		gFL_clipStackDepth--;
		fl_pop_clip();
	}

	struct GCState
	{
		void init();
		void validate();

		inline void applyColor()
		{
			fl_color(color);
		}
		inline void applyLineStyle()
		{
			fl_line_style(
				lineStyle|lineCap|lineJoin,
				lineWidth,
				0
				);
		}
		inline void applyFont()
		{
			fl_font(font.id, font.size);
		}
		inline void apply()
		{
			applyColor();
			applyLineStyle();
			applyFont();
		}

		Fl_Color		color;
		int				lineWidth;
		int				lineStyle;
		int				lineCap;
		int				lineJoin;
		struct {
			int			id;
			int			size;
		}				font;
		int				clipped;
		SCUM_Rect		clip;
	};

	static GCState gGCState;
	static std::vector<GCState> gGCStateStack;

	// =====================================================================
	// Util

	inline static SCUM::ModMask FL_state()
	{
		int flstate = Fl::event_state();
		int state = 0;

		if (flstate & FL_SHIFT)
			state |= SCUM::kModMaskShift;
		if (flstate & FL_CTRL)
			state |= SCUM::kModMaskControl;
		if ((flstate & FL_ALT) || (flstate & FL_META))
			state |= SCUM::kModMaskCommand;

		return (SCUM::ModMask)state;
	}

	inline static SCUM_Point FL_eventPos()
	{
		return SCUM_Point(Fl::event_x(), Fl::event_y());
	}

	static wint_t FL_eventKey(int& state)
	{
		const int key = Fl::event_key();
		
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
			state |= SCUM::kModMaskKeypad;
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
				state |= SCUM::kModMaskShift;
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
				state |= SCUM::kModMaskShift;
				return 0;
			case FL_Control_L:
			case FL_Control_R:
				state |= SCUM::kModMaskControl;
				return 0;
			case FL_Caps_Lock:
				return 0;
			case FL_Alt_L:
			case FL_Alt_R:
			case FL_Meta_L:
			case FL_Meta_R:
				state |= SCUM::kModMaskCommand;
				return 0;
			case FL_Menu: return NSMenuFunctionKey;
			case FL_Num_Lock:
				return 0;
			case FL_KP_Enter:
				state |= SCUM::kModMaskKeypad;
				return 0x0a;
		}

		return 0;
	}

	inline static int FL_fontId(const char* name)
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
}; // SCUM

// =====================================================================
// SCUM::FLFontHandle

SCUM::FontHandle* SCUM::FontHandle::create(const char* name, float size)
{
	return new SCUM::FLFontHandle(name, size);
}

SCUM::FLFontHandle::FLFontHandle(const char* name, float size)
	: m_size((int)size),
	  m_id(SCUM::FL_fontId(name))
{
}

inline void SCUM::FLFontHandle::restoreFont(int id, int size)
{
	fl_font(id, size);
}

inline void SCUM::FLFontHandle::saveFont(int& id, int& size)
{
	id = fl_font();
	size = fl_size();
	restoreFont(m_id, m_size);
}

float SCUM::FLFontHandle::height()
{
	int id, size;
	float res;

	saveFont(id, size);
	restoreFont(m_id, m_size);
	res = (float)fl_height();
	restoreFont(id, size);

	return res;
}

float SCUM::FLFontHandle::ascent()
{
	int id, size;
	float res;

	saveFont(id, size);
	restoreFont(m_id, m_size);
	res = (float)(fl_height() - fl_descent());
	restoreFont(id, size);

	return res;
}

float SCUM::FLFontHandle::descent()
{
	int id, size;
	float res;

	saveFont(id, size);
	restoreFont(m_id, m_size);
	res = (float)fl_descent();
	restoreFont(id, size);

	return res;
}

SCUM_Size SCUM::FLFontHandle::measure(const char* str)
{
	int id, size;
	int w=0, h=0;
	
	saveFont(id, size);
	restoreFont(m_id, m_size);
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
	clipped = 0;
}

void SCUM::GCState::validate()
{
	if (!gGCStateStack.empty()) {
		fprintf(stderr, "SCUM: GC state stack non empty!\n");
		while (!gGCStateStack.empty()) {
			GCRestore();
		}
	}
	if (clipped != 0) {
		fprintf(stderr, "SCUM: invalid clip stack depth %d\n", clipped);
		abort();
	}
}

void SCUM::init()
{
	gGCState.init();
}

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

// =====================================================================
// SCUM_FLWindow

SCUM::WindowHandle* SCUM::WindowHandle::create(SCUM_Window* view)
{
	return new SCUM::FLWindowHandle(view);
}

SCUM::FLWindowHandle::FLWindowHandle(SCUM_Window* view)
	: SCUM_FL_WINDOW_HANDLE_SUPER(0, 0, 0, 0),
	  m_view(view)
{
	end();
	callback(closeCB, view);
	free_position();

	m_flags.shown = shown();
	m_flags.visible = visible();
	m_flags.focused = Fl::focus() == this;
	m_flags.decorated = border();
	m_flags.modal = modal();
	m_flags.fullscreen = false;
}

SCUM::FLWindowHandle::~FLWindowHandle()
{
}

SCUM::WindowHandle::Flags SCUM::FLWindowHandle::flags()
{
	return m_flags;
}

void SCUM::FLWindowHandle::setFlags(const Flags& flags)
{
	if (m_flags.decorated != flags.decorated) {
		m_flags.decorated = flags.decorated;
		border(flags.decorated);
	}
	if (m_flags.modal != flags.modal) {
		m_flags.modal = flags.modal;
		if (flags.modal) set_modal();
		else set_non_modal();
	}
	if (m_flags.fullscreen != flags.fullscreen) {
		m_flags.fullscreen = flags.fullscreen;
		if (flags.fullscreen) {
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

SCUM_Rect SCUM::FLWindowHandle::bounds()
{
	return SCUM_Rect(x(), y(), w(), h());
}

void SCUM::FLWindowHandle::setTitle(const char* str)
{
	m_title = str;
	label(m_title.c_str());
}

void SCUM::FLWindowHandle::setPos(const SCUM_Point& pos)
{
}

void SCUM::FLWindowHandle::setSize(const SCUM_Size& size)
{
	SCUM_FL_WINDOW_HANDLE_SUPER::size((int)size.w, (int)size.h);
}

void SCUM::FLWindowHandle::setMinSize(const SCUM_Size& size)
{
	m_minSize = size;
	size_range(
		(int)size.w, (int)size.h,
		(int)m_maxSize.w, (int)m_maxSize.h
		);
}

void SCUM::FLWindowHandle::setMaxSize(const SCUM_Size& size)
{
	m_maxSize = size;
	size_range(
		(int)m_minSize.w, (int)m_minSize.h,
		(int)size.w, (int)size.h
		);
}

// void SCUM::FLWindowHandle::destroy()
// {
// 	// IMPORTANT: since some virtual SCUM_WindowHandle methods are
// 	// called from view destructors, _first_ the view has to be
// 	// deleted, _then_ the handle. here is where it happens.
// 	delete m_view;
// 	delete this;
// }

void SCUM::FLWindowHandle::show()
{
	SCUM_FL_WINDOW_HANDLE_SUPER::show();
}

void SCUM::FLWindowHandle::hide()
{
	SCUM_FL_WINDOW_HANDLE_SUPER::hide();
}

void SCUM::FLWindowHandle::refresh(const SCUM_Rect& damage)
{
	std::cout << "refresh: " << m_damage << " " << damage << " " << (m_damage | damage) << "\n";
	SCUM_FL_WINDOW_HANDLE_SUPER::damage(
		FL_DAMAGE_EXPOSE,
		(int)damage.x, (int)damage.y,
		(int)damage.w, (int)damage.h
		);
	m_damage = m_damage | damage;
}

int SCUM::FLWindowHandle::handle(int evt)
{
	switch (evt) {
		case FL_PUSH:
			if (Fl::event_button() == 3) {
				m_view->contextMenu(SCUM::FL_state(), SCUM::FL_eventPos());
			} else {
				m_view->mouseDown(SCUM::FL_state(), SCUM::FL_eventPos());
			}
			return true;
		case FL_DRAG:
			m_view->mouseMove(SCUM::FL_state(), SCUM::FL_eventPos());
			return true;
		case FL_RELEASE:
			m_view->mouseUp(SCUM::FL_state(), SCUM::FL_eventPos());
			return true;
		case FL_MOUSEWHEEL:
		{
			float dx = Fl::event_dx();
			float dy = Fl::event_dy();
			int state = SCUM::FL_state();
			if (state & SCUM::kModMaskControl) {
				// swap axes
				float tmp = dx;
				dx = dy;
				dy = tmp;
				// strip modifier
				state &= ~SCUM::kModMaskControl;
			}
			m_view->scrollWheel(state, SCUM::FL_eventPos(), SCUM_Point(dx, dy));
		}
		return true;
		case FL_FOCUS:
			if (!m_flags.focused) {
				m_flags.focused = true;
				m_view->focusEvent(true);
			}
			return true;
		case FL_UNFOCUS:
			if (m_flags.focused) {
				m_flags.focused = false;
				m_view->focusEvent(false);
			}
			return true;
		case FL_KEYDOWN:
		{
			int state = SCUM::FL_state();
			wint_t key = SCUM::FL_eventKey(state);
			m_view->keyDown(state, key);
		}
		return true;
		case FL_KEYUP:
		{
			int state = SCUM::FL_state();
			wint_t key = SCUM::FL_eventKey(state);
			m_view->keyUp(state, key);
		}
		return true;
		case FL_SHOW:
			m_flags.shown = true;
			m_flags.visible = true;
			m_view->showEvent();
			return SCUM_FL_WINDOW_HANDLE_SUPER::handle(evt);
		case FL_HIDE:
			m_flags.visible = false;
			m_view->hideEvent();
			return true;
	}

	return SCUM_FL_WINDOW_HANDLE_SUPER::handle(evt);
}

void SCUM::FLWindowHandle::resize(int x, int y, int w, int h)
{
	SCUM_FL_WINDOW_HANDLE_SUPER::resize(x, y, w, h);
	m_view->resizeEvent(SCUM_Rect(x, y, w, h));
}

void SCUM::FLWindowHandle::draw()
{
	GCSave();
	if (!m_damage.isEmpty()) {
		GCSetClip(m_damage);
		m_damage = SCUM_Rect();
	}
	m_view->draw();
	GCRestore();
	gGCState.validate();
}

void SCUM::FLWindowHandle::closeCB(Fl_Widget* w, void* arg)
{
	((SCUM_Window*)arg)->closeEvent();
}

// =====================================================================
// SCUM_FLGLContext

SCUM::GLContext* SCUM::GLContext::create(SCUM::WindowHandle* window, SCUM_View* view)
{
	return new FLGLContext(dynamic_cast<Fl_Group*>(window), view);
}

SCUM::GLContext* SCUM::GLContext::create(SCUM_View* view)
{
	return SCUM::GLContext::create(view->window()->handle(), view);
}

SCUM::FLGLContext::FLGLContext(Fl_Group* parent, SCUM_View* view)
	: Fl_Gl_Window(0, 0, 0, 0),
	  m_view(view)
{
	mode(FL_RGB|FL_DOUBLE|FL_DEPTH);
	invalidate();
	resize(0, 0, 0, 0);
	parent->add(this);
}

SCUM::FLGLContext::~FLGLContext()
{
	parent()->remove(this);
}

SCUM_Rect SCUM::FLGLContext::bounds()
{
	return SCUM_Rect(x(), y(), w(), h());
}

void SCUM::FLGLContext::setBounds(const SCUM_Rect& bounds)
{
	resize((int)bounds.x, (int)bounds.y, (int)bounds.w, (int)bounds.h);
}

void SCUM::FLGLContext::refresh()
{
	redraw();
}

int SCUM::FLGLContext::handle(int evt)
{
	if (evt == FL_SHOW)
		return Fl_Gl_Window::handle(evt);
	return 0;
}

void SCUM::FLGLContext::draw()
{
	if (!valid()) {
		m_view->initGL();
		m_view->resizeGL(w(), h());
	}
	m_view->drawGL();
}

// =====================================================================
// SCUM_FLMenu

SCUM::MenuHandle* SCUM::MenuHandle::create(const std::vector<SCUM_MenuItem>& items)
{
	return new SCUM::FLMenuHandle(items);
}

SCUM::FLMenuHandle::FLMenuHandle(const std::vector<SCUM_MenuItem>& items)
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

SCUM::FLMenuHandle::~FLMenuHandle()
{
	delete [] m_items;
	delete [] m_data;
}

bool SCUM::FLMenuHandle::value(int item)
{
	return (item >= 0) && (item < m_size) && m_data[item].value();
}

void SCUM::FLMenuHandle::setValue(int item, bool value)
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

int SCUM::FLMenuHandle::popup(const SCUM_Point& where, int item)
{
	const Fl_Menu_Item* data
		= (item >= 0) && (item < m_size) ? m_data + item : 0;

	const Fl_Menu_Item* res
		= m_data->popup((int)where.x, (int)where.y, 0, data);

	return res ? ((Item*)res->user_data())->index : -1;
}

// =====================================================================
// SCUM_FLImage

SCUM::ImageHandle* SCUM::ImageHandle::create(const char* fileName, const SCUM_Size& size)
{
	return new SCUM::FLImageHandle(fileName, size);
}

SCUM::FLImageHandle::FLImageHandle(const char* fileName, const SCUM_Size& size)
{
	int w = (int)size.w;
	int h = (int)size.h;
	m_image = Fl_Shared_Image::get(fileName, w, h);
}

SCUM::FLImageHandle::~FLImageHandle()
{
	if (m_image) m_image->release();
}

SCUM_Size SCUM::FLImageHandle::size()
{
	if (m_image) return SCUM_Size(m_image->w(), m_image->h());
	return SCUM_Size();
}

// =====================================================================
// SCUM::GC

void SCUM::GCSave()
{
	gGCStateStack.push_back(gGCState);
}

void SCUM::GCRestore()
{
	if (gGCState.clipped > 0) {
		gGCState.clipped--;
		FL_popClip();
	}
	if (gGCState.clipped > 0) {
		fprintf(stderr, "SCUM: invalid clip stack depth\n");
		abort();
	}
	gGCState = gGCStateStack.back();
	gGCState.apply();
	gGCStateStack.pop_back();
}

void SCUM::GCSetColor(const SCUM_Color& c)
{
	gGCState.color = fl_rgb_color(c.r8(), c.g8(), c.b8());
	gGCState.applyColor();
}

void SCUM::GCSetLineWidth(float width)
{
	gGCState.lineWidth = (int)width;
	gGCState.applyLineStyle();
}

void SCUM::GCSetLineStyle(SCUM::LineStyle style)
{
	switch (style) {
		case SCUM::kLineStyleSolid: gGCState.lineStyle = FL_SOLID; break;
		case SCUM::kLineStyleDash: gGCState.lineStyle = FL_DASH; break;
		case SCUM::kLineStyleDot: gGCState.lineStyle = FL_DOT; break;
	}
	gGCState.applyLineStyle();
}

void SCUM::GCSetLineCap(SCUM::LineCap cap)
{
	switch (cap) {
		case SCUM::kLineCapFlat: gGCState.lineCap = FL_CAP_FLAT; break;
		case SCUM::kLineCapRound: gGCState.lineCap = FL_CAP_ROUND; break;
		case SCUM::kLineCapSquare: gGCState.lineCap = FL_CAP_SQUARE; break;
	}
	gGCState.applyLineStyle();
}

void SCUM::GCSetLineJoin(SCUM::LineJoin join)
{
	switch (join) {
		case SCUM::kLineJoinMiter: gGCState.lineJoin = FL_JOIN_MITER; break;
		case SCUM::kLineJoinRound: gGCState.lineJoin = FL_JOIN_ROUND; break;
		case SCUM::kLineJoinBevel: gGCState.lineJoin = FL_JOIN_BEVEL; break;
	}
	gGCState.applyLineStyle();
}

void SCUM::GCSetFont(SCUM::FontHandle* fontHandle)
{
	SCUM::FLFontHandle* font = dynamic_cast<SCUM::FLFontHandle*>(fontHandle);
	if (font) {
		gGCState.font.id = font->m_id;
		gGCState.font.size = font->m_size;
		gGCState.applyFont();
	}
}

void SCUM::GCDrawPoint(float x, float y)
{
	fl_point((int)x, (int)y);
}

void SCUM::GCDrawLine(float x1, float y1, float x2, float y2)
{
	fl_line((int)x1, (int)y1, (int)x2, (int)y2);
}

void SCUM::GCDrawRect(float x, float y, float w, float h)
{
	fl_rect((int)x, (int)y, (int)w, (int)h);
}

namespace SCUM
{
	inline void GCDrawBeveledRectNW(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_yxline(x1++, y2--, y1++, x2--);
	}

	inline void GCDrawBeveledRectSE(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_xyline(x1++, y2--, x2--, y1++);
	}
};

void SCUM::GCDrawBeveledRect(float xf, float yf, float wf, float hf, float bwf, bool inout)
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
	GCDrawBeveledRectSE(x1, y1, x2, y2, bw);

	// draw NW (overlaps SE)
	if (inout) fl_color(dc);
	else fl_color(lc);
	GCDrawBeveledRectNW(x1, y1, x2, y2, bw);

	// restore color
	fl_color(bc);
}

void SCUM::GCFillRect(float x, float y, float w, float h)
{
	fl_rectf((int)x, (int)y, (int)w, (int)h);
}

void SCUM::GCDrawPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_loop((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::GCFillPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_polygon((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::GCDrawArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_arc((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::GCFillArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_pie((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::GCDrawText(float x, float y, const char* str)
{
	fl_draw(str, (int)x, (int)y);
}

void SCUM::GCInitClip()
{
	if (gGCState.clipped > 0) {
		FL_popClip();
		gGCState.clipped--;
	}
	if (gGCState.clipped > 0) {
		fprintf(stderr, "SCUM: invalid clip stack depth\n");
		abort();
	}
	gGCState.clipped++;
	FL_pushNoClip();
}

void SCUM::GCSetClip(const SCUM_Rect& r)
{
	if (gGCState.clipped > 0) {
		FL_popClip();
		gGCState.clipped--;
	}
	if (gGCState.clipped > 0) {
		fprintf(stderr, "SCUM: invalid clip stack depth\n");
		abort();
	}
	gGCState.clipped++;
// 	gGCState.clip = r;
	FL_pushClip(r);
}

bool SCUM::GCIsClipped(const SCUM_Rect& r)
{
	return !fl_not_clipped((int)r.x, (int)r.y, (int)r.w, (int)r.h);
}

SCUM_Rect SCUM::GCClippedRect(const SCUM_Rect& r)
{
	int x, y, w, h;
	if (fl_clip_box((int)r.x, (int)r.y, (int)r.w, (int)r.h, x, y, w, h))
		return SCUM_Rect(x, y, w, h);
	return r;
}

// EOF
