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


#ifndef SCUM_GL_HH_INCLUDED
#define SCUM_GL_HH_INCLUDED

#include "SCUM_Color.hh"
#include "SCUM_Geometry.hh"
#include "SCUM_View.hh"

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

// =====================================================================
// SCUM_GLContext

class SCUM_GLView;

class SCUM_GLContext : public Fl_Gl_Window
{
public:
    SCUM_GLContext(SCUM_GLView* view);
    virtual ~SCUM_GLContext();

    inline SCUM_Rect bounds() const;
    inline void setBounds(const SCUM_Rect& bounds);
    void refresh();

protected:
    virtual int handle(int);
    virtual void draw();

private:
    SCUM_GLView*	m_view;
};

// =====================================================================
// SCUM_GLView

class SCUM_GLView : public SCUM_View
{
    friend class SCUM_GLContext;

public:
    SCUM_GLView(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
    virtual ~SCUM_GLView();

    virtual void refresh(const SCUM_Rect& damage);
    virtual void refresh();
    virtual void drawView(const SCUM_Rect& damage);
    virtual void setBounds(const SCUM_Rect& bounds);
    virtual SCUM_Size getMinSize();

    virtual void setProperty(const char* key, SCUM_ArgStream& args);

    virtual void initGL();
    virtual void drawGL();

    inline SCUM_GLContext* context();

    inline SCUM_Point padding() const;
    inline void setPadding(const SCUM_Point& padding);

    inline SCUM::Border border() const;
    inline void setBorder(SCUM::Border border);

protected:
    void releaseContext() { m_context = 0; }

private:
    SCUM_GLContext*	m_context;
    SCUM_Point		m_padding;
    uint8_t		m_border;
};

// =====================================================================
// SCUM_GLContext (inline functions)

inline SCUM_Rect SCUM_GLContext::bounds() const
{
    return SCUM_Rect(x(), y(), w(), h());
}

inline void SCUM_GLContext::setBounds(const SCUM_Rect& bounds)
{
    resize(bounds.xi(), bounds.yi(), bounds.wi(), bounds.hi());
}

// =====================================================================
// SCUM_GLView (inline functions)

inline SCUM_GLContext* SCUM_GLView::context()
{
    return m_context;
}

inline SCUM_Point SCUM_GLView::padding() const
{
    return m_padding;
}

inline void SCUM_GLView::setPadding(const SCUM_Point& padding)
{
    m_padding = padding;
    updateLayout();
}

inline SCUM::Border SCUM_GLView::border() const
{
    return (SCUM::Border)m_border;
}

inline void SCUM_GLView::setBorder(SCUM::Border border)
{
    m_border = border;
    updateLayout();
}

// =====================================================================
// Util

namespace SCUM
{
    inline static void glColor(const SCUM_Color& color)
    {
	::glColor4ub(color.r8(), color.g8(), color.b8(), color.a8());
    }

    inline static void glClearColor(const SCUM_Color& color)
    {
	::glClearColor(color.r32(), color.g32(), color.b32(), color.a32());
    }

    inline static void glVertex(const SCUM_Point& p)
    {
	::glVertex2f(p.x, p.y);
    }

    inline static void glRect(const SCUM_Rect& r)
    {
	::glRectf(r.minX(), r.minY(), r.maxX(), r.maxY());
    }

    inline static void glTranslate(const SCUM_Point& p)
    {
	::glTranslatef(p.x, p.y, 0.0f);
    }

    inline static void glRasterPos(const SCUM_Point& p)
    {
	::glRasterPos2f(p.x, p.y);
    }

    inline static void glScissor(const SCUM_Rect& scissor)
    {
	::glScissor(
		    (GLint)scissor.x,
		    (GLint)scissor.y,
		    (GLint)scissor.w,
		    (GLint)scissor.h
		    );
    }
};

#endif // SCUM_GL_HH_INCLUDED
