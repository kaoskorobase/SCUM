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

	$Id: SCUM_System.cpp,v 1.2 2004/08/15 14:42:24 steve Exp $
*/


#include "SCUM_FLApp.hh"
#include "SCUM_Image.hh"
#include "SCUM_Menu.hh"
#include "SCUM_System.hh"
#include "SCUM_Window.hh"
#include "SCUM_Util.hh"

#include <FL/Fl.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>

#include <ctype.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace SCUM
{
	// =====================================================================
	// SCUM::FL::FontHandle

#if 0
	class FLFontHandle : public FontHandle
	{
	public:
		FLFontHandle(int id, int size);

		virtual void extents(SCUM_FontExtents& res);
		virtual void measure(const char* str, SCUM_TextExtents& res);
		
		inline void saveFont(int& id, int& size);
		inline void restoreFont(int id, int size);

		int			m_id;
		int			m_size;
	};

	// =====================================================================
	// SCUM_FLWindow

	class FLWindowHandle : public SCUM_FL_WINDOW_HANDLE_SUPER, public WindowHandle
	{
	public:
		FLWindowHandle(SCUM_Window* view);
		
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
		virtual void destroy();

	private:
		static void closeCB(Fl_Widget* w, void*);

	private:
		SCUM_Window*	m_view;
		Flags			m_flags;
		std::string		m_title;
		SCUM_Size		m_minSize;
		SCUM_Size		m_maxSize;
		SCUM_Rect		m_saveBounds;
	};
#endif

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
}; // SCUM
#if 0

// =====================================================================
// SCUM::FLFontHandle

namespace SCUM
{
	struct FLFontDesc
	{
		const char* name;
		int id;
	};

	static FLFontDesc gFontTable[] = {
		{ "Helvetica", FL_HELVETICA },
		{ "Arial", FL_HELVETICA },
		{ "sans", FL_HELVETICA },
		{ "Times", FL_TIMES },
		{ "serif", FL_TIMES },
		{ "Courier", FL_COURIER },
		{ "Monaco", FL_COURIER },
		{ "monospace", FL_COURIER }
	};
	static const int kNumFonts = sizeof(gFontTable)/sizeof(FLFontDesc);

	static bool getFontID(const char* name, int& id)
	{
		bool found = false;
		
		const char* end = strchr(name, '-');
		size_t len = end ? end - name : strlen(name);

		for (int i=0; i < kNumFonts; i++) {
			if (strncmp(name, gFontTable[i].name, len) == 0) {
				id = gFontTable[i].id;
				found = true;
				break;
			}
		}
		
		if (!found) return false;

		while (true) {
			name = strchr(name, '-');
			if (!name) break;
			name++;
			if (strncmp(name, "Bold", 4) == 0) {
				id |= FL_BOLD;
			} else if (strncmp(name, "Italic", 6) == 0) {
				id |= FL_ITALIC;
			}
		}
		
		return true;
	}
}

SCUM::FontHandle* SCUM::FontHandle::create(const char* name, double size)
{
	int id;
	if (getFontID(name, id)) {
		return new SCUM::FLFontHandle(id, (int)size);
	}
	return 0;
}

SCUM::FLFontHandle::FLFontHandle(int id, int size)
	: m_size(size),
	  m_id(id)
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

void SCUM::FLFontHandle::extents(SCUM_FontExtents& res)
{
	int id, size;
	saveFont(id, size);
	res.height = fl_height();
	res.descent = fl_descent();
	res.ascent = res.height - res.descent;
	restoreFont(id, size);
}

void SCUM::FLFontHandle::measure(const char* str, SCUM_TextExtents& res)
{
	int id, size;
	int w=0, h=0;
	
	saveFont(id, size);
	restoreFont(m_id, m_size);
	fl_measure(str, w, h, 0);
	restoreFont(id, size);

	res.size.w = w;
	res.size.h = h;
	res.bearing = SCUM_Point();
	res.advance = SCUM_Point();
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

void SCUM::FLWindowHandle::destroy()
{
	delete m_view;
	m_view = 0;
	delete this;
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
	this->damage(
		FL_DAMAGE_CHILD,
		(int)damage.x, (int)damage.y,
		(int)damage.w, (int)damage.h
		);
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
	int x, y, w, h;
	if (!fl_clip_box(0, 0, this->w(), this->h(), x, y, w, h)) {
	} else {
		x = y = 0;
		w = this->w();
		h = this->h();
	}
	m_view->draw(SCUM_Rect(x, y, w, h));
}

void SCUM::FLWindowHandle::closeCB(Fl_Widget* w, void* arg)
{
	((SCUM_Window*)arg)->closeEvent();
}
#endif

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

// splash

namespace SCUM
{
	class FLSplash : public Fl_Window
	{
	public:
		FLSplash(const char* label, double timeout)
			: Fl_Window(0, 0, label)
		{
			m_img = new Fl_PNG_Image("/tmp/scum.png");
			size(m_img->w(), m_img->h());
			addTimeout(timeout);
			clear_border();
			set_modal();
			hotspot(this);
			show();
		}
		~FLSplash()
		{
			SCUM_DEBUG_PRINT("yay!\n");
			addTimeout(0);
			delete m_img;
		}

		int handle(int evt)
		{
			switch (evt) {
				case FL_KEYDOWN:
					if (Fl::event_key() == FL_Escape)
						goto yay;
					break;
				case FL_PUSH:
					goto yay;
			}

			return Fl_Window::handle(evt);

		yay:
			addTimeout(0.01);
			return 0;
		};

		void draw()
		{
			m_img->draw(0, 0);
		}

		void addTimeout(double timeout)
		{
			if (Fl::has_timeout(timeoutCB))
				Fl::remove_timeout(&timeoutCB, this);
			if (timeout > 0) {
				Fl::add_timeout(timeout, &timeoutCB, this);
			}
		}
		static void timeoutCB(void* self)
		{
			delete (FLSplash*)self;
		}

		Fl_PNG_Image* m_img;
	};

	void showSplash(const char* label, double timeout)
	{
		new FLSplash(label, timeout);
	}
};

// =====================================================================
// SCUM::System

#include <time.h>
#include <sys/time.h>

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
			delete timer;
		}
	}
};

void SCUM::addTimer(SCUM_Timer* timer)
{
	Fl::add_timeout(timer->timeout(), &timerCB, timer);
}

// EOF
