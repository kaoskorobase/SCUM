/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vi: et sta sw=4:

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


#ifndef SCUM_CANVAS_HH_INCLUDED
#define SCUM_CANVAS_HH_INCLUDED

#include "SCUM_Color.hh"
#include "SCUM_Geometry.hh"
#include "SCUM_Handle.hh"
#include "SCUM_View.hh"
#include "SCUM_Window.hh"

class SCUM_CanvasView;
typedef struct cairo cairo_t;

namespace SCUM
{
    class CanvasHandle : public SCUM_Handle
    {
    public:
        static CanvasHandle* create(WindowHandle* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);

        virtual void setContext(cairo_t* cr);
        virtual SCUM_Rect bounds() = 0;
        virtual void setBounds(const SCUM_Rect& bounds) = 0;
        virtual void refresh() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
    };

    void cairo_set_rgb_color(cairo_t* cr, const SCUM_Color& c);
    void cairo_set_color(cairo_t* cr, const SCUM_Color& c);

    void cairo_move_to(cairo_t* cr, const SCUM_Point& p);
    void cairo_line_to(cairo_t* cr, const SCUM_Point& p);
    void cairo_rectangle(cairo_t* cr, const SCUM_Rect& r);
};

class SCUM_CanvasView : public SCUM_View
{
public:
    SCUM_CanvasView(SCUM_Container* parent, PyrObject* obj);
    virtual ~SCUM_CanvasView();

    virtual void refresh(const SCUM_Rect& damage);
    virtual void refresh();
    virtual void draw(const SCUM_Rect& damage);
    virtual void setBounds(const SCUM_Rect& bounds);
    virtual SCUM_Size getMinSize();
    virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);

    inline void setPadding(const SCUM_Point& padding);
    inline const SCUM_Point& padding() const;
	
    virtual void initCanvas();
    virtual void drawCanvas();
    inline SCUM_Rect canvasBounds() const;

protected:
    inline cairo_t* cairo();

private:
    SCUM::CanvasHandle*	m_handle;
    cairo_t*			m_cairo;
    SCUM_Point			m_padding;
};

inline void SCUM_CanvasView::setPadding(const SCUM_Point& padding)
{
    m_padding = padding;
    updateLayout();
}

inline const SCUM_Point& SCUM_CanvasView::padding() const
{
    return m_padding;
}

inline SCUM_Rect SCUM_CanvasView::canvasBounds() const
{
    return m_handle ? m_handle->bounds() : SCUM_Rect(bounds().inset(m_padding));
}

inline cairo_t* SCUM_CanvasView::cairo()
{
    return m_cairo;
}

#endif // SCUM_CANVAS_HH_INCLUDED
