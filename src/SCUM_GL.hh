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

	$Id: SCUM_GL.hh,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#ifndef SCUM_GL_HH_INCLUDED
#define SCUM_GL_HH_INCLUDED

#include <GL/gl.h>
#include <GL/glu.h>

#include "SCUM_Color.hh"
#include "SCUM_Geometry.hh"
#include "SCUM_View.hh"
#include "SCUM_Window.hh"

// =====================================================================
// SCUM::GLContext

class SCUM_GLView;

// class SCUM_GLContext
// {
// public:
// 	SCUM_GLContext(SCUM_View* view);
// 	virtual ~SCUM_GLContext();

// 	SCUM_View* view() { return m_view; }

// 	virtual SCUM_Rect bounds() = 0;
// 	virtual void setBounds(const SCUM_Rect& bounds) = 0;
// 	virtual void refresh() = 0;

// private:
// 	SCUM_View*	m_view;
// };

namespace SCUM
{
// 	SCUM_GLContext* makeGLContext(SCUM_WindowHandle* window, SCUM_View* view);
// 	SCUM_GLContext* makeGLContext(SCUM_View* view);

	class GLContext
	{
	public:
		static GLContext* create(SCUM::WindowHandle* window, SCUM_View* view);
		static GLContext* create(SCUM_View* view);

		virtual SCUM_Rect bounds() = 0;
		virtual void setBounds(const SCUM_Rect& bounds) = 0;
		virtual void refresh() = 0;
// 		virtual void drawText(FontHandle* font, const char* str) = 0;
	}; // GLContext
};

// =====================================================================
// SCUM_GLView

class SCUM_GLView : public SCUM_View
{
public:
	SCUM_GLView(SCUM_Container* parent, PyrObject* obj);
	virtual ~SCUM_GLView();

	void refreshGL();
	
protected:
	SCUM::GLContext* context() { return m_context; }

private:
	SCUM::GLContext*	m_context;
};

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

// inline void glStrokeRect(const SCUM_Rect& r)
// {
// 	GLfloat lw;
// 	glGetFloatv(GL_LINE_WIDTH, &lw);
// 	glRectf(r.origin.x, r.origin.y, r.extent.x, r.origin.y+lw);   // s
// 	glRectf(r.origin.x, r.origin.y + lw, r.origin.x + lw, r.extent.y - lw); // w
// 	glRectf(r.extent.x - lw, r.origin.y + lw, r.extent.x, r.extent.y - lw); // e
// 	glRectf(r.origin.x, r.extent.y - lw, r.extent.x, r.extent.y); // n
// }

// inline void glLineLoop(const SCUM_Rect& r)
// {
// 	glBegin(GL_LINE_LOOP);
// 	glVertex2f(r.origin.x, r.origin.y);
// 	glVertex2f(r.extent.x, r.origin.y);
// 	glVertex2f(r.extent.x, r.extent.y);
// 	glVertex2f(r.origin.x, r.extent.y);
// 	glEnd();
// }

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
