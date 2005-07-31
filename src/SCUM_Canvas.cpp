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


#include "SCUM_Canvas.hh"
#include "SCUM_GC.hh"
#include "SCUM_System.hh"
#include "SCUM_Util.hh"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#if HAVE_CAIRO
# include <cairo.h>
#else
struct cairo {};
#endif // HAVE_CAIRO

using namespace SCUM;

SCUM_CanvasView::SCUM_CanvasView(SCUM_Container* parent, PyrObject* obj)
    : SCUM_View(parent, obj),
      m_handle(0),
      m_cairo(0)
{
#if HAVE_CAIRO
    m_cairo = cairo_create();
    CheckAlloc(m_cairo);
    m_handle = SCUM::CanvasHandle::create(window()->handle(), this, m_cairo, true);
    if (m_handle) m_handle->setContext(m_cairo);
#endif
}

SCUM_CanvasView::~SCUM_CanvasView()
{
    if (m_handle) m_handle->release();
}

void SCUM_CanvasView::setBounds(const SCUM_Rect& bounds)
{
    SCUM_View::setBounds(bounds);
    if (m_handle) m_handle->setBounds(bounds.inset(m_padding));
}

SCUM_Size SCUM_CanvasView::getMinSize()
{
    return SCUM_View::getMinSize().padded(m_padding);
}

void SCUM_CanvasView::refresh()
{
    if (m_handle) m_handle->refresh();
}

void SCUM_CanvasView::refresh(const SCUM_Rect& /* damage */)
{
    if (m_handle) m_handle->refresh();
}

void SCUM_CanvasView::draw(const SCUM_Rect& damage)
{
    GCSave();
    GCSetColor(bgColor());
    GCFillRect(bounds());
    GCRestore();
}

void SCUM_CanvasView::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (SCUM::equal(key, "visible")) {
        if (m_handle) {
            bool vis0 = flags().vVisible;
            SCUM_View::setProperty(key, slot);
            bool vis1 = flags().vVisible;
            if (vis0 != vis1) {
                if (vis1) {
                    m_handle->show();
                } else {
                    m_handle->hide();
                }
            }
        }
    } else {
        SCUM_View::setProperty(key, slot);
    }
}

void SCUM_CanvasView::initCanvas()
{
}

void SCUM_CanvasView::drawCanvas()
{
#if HAVE_CAIRO
    cairo_t* cr = cairo();
    SCUM_Rect bounds(0, 0, canvasBounds().w, canvasBounds().h);

    cairo_save(cr);
    cairo_set_rgb_color(cr, bgColor());
    cairo_rectangle(cr, bounds);
    cairo_fill(cr);
    cairo_set_rgb_color(cr, 1, 0, 0);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, bounds.maxX(), bounds.maxY());
    cairo_move_to(cr, 0, bounds.maxY());
    cairo_line_to(cr, bounds.maxX(), 0);
    cairo_stroke(cr);
    cairo_restore(cr);

#if 0
    cairo_save(cr);
    cairo_scale(cr, bounds.w, bounds.h);

    cairo_save(cr);
    cairo_scale(cr, 1./bounds.w, 1./bounds.h);

    cairo_rectangle(cr, 0, 0, 1, 1);
    cairo_set_rgb_color(cr, bgColor());
    cairo_fill(cr);

    cairo_set_rgb_color(cr, fgColor());
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, 0, 0);
    // 	cairo_line_to(cr, bounds.maxX(), bounds.maxY());
    // 	cairo_move_to(cr, 0, bounds.maxY());
    // 	cairo_line_to(cr, bounds.maxX(), 0);
    cairo_arc(cr, 0.5, 0.5, 0.4, 0, M_2_PI);
    cairo_stroke(cr);

    cairo_restore(cr);

    cairo_restore(cr);
#endif
#endif // HAVE_CAIRO
}

#if HAVE_CAIRO
void SCUM::cairo_set_rgb_color(cairo_t* cr, const SCUM_Color& c)
{
    ::cairo_set_rgb_color(cr, c.r32(), c.g32(), c.b32());
}

void SCUM::cairo_set_color(cairo_t* cr, const SCUM_Color& c)
{
    ::cairo_set_rgb_color(cr, c.r32(), c.g32(), c.b32());
    ::cairo_set_alpha(cr, c.a32());
}

void SCUM::cairo_move_to(cairo_t* cr, const SCUM_Point& p)
{
    ::cairo_move_to(cr, p.x, p.y);
}

void SCUM::cairo_line_to(cairo_t* cr, const SCUM_Point& p)
{
    ::cairo_line_to(cr, p.x, p.y);
}

void SCUM::cairo_rectangle(cairo_t* cr, const SCUM_Rect& r)
{
    ::cairo_rectangle(cr, r.x, r.y, r.w, r.h);
}
#endif // HAVE_CAIRO

// EOF
