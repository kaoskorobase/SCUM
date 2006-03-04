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
	case SCUM::kMenuSeparator:
	    data->flags = FL_MENU_INVISIBLE;
	    if (lastFlags) *lastFlags |= FL_MENU_DIVIDER;
	    break;
	case SCUM::kMenuToggle:
	    data->flags = FL_MENU_TOGGLE;
	    break;
	case SCUM::kMenuRadio:
	    data->flags = FL_MENU_RADIO;
	    break;
	case SCUM::kMenuOpen:
	    data->flags = FL_SUBMENU;
	    break;
	case SCUM::kMenuClose:
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

#if 0
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
#endif

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
