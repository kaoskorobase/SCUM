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

	$Id: SCUM_GL.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_GL.hh"
#include "SCUM_System.hh"

#include <assert.h>

using namespace SCUM;

// =====================================================================
// SCUM_GLContext

// SCUM_GLContext::SCUM_GLContext(SCUM_View* view)
// 	: m_view(view)
// { }

// SCUM_GLContext::~SCUM_GLContext()
// { }

// =====================================================================
// SCUM_GLView

SCUM_GLView::SCUM_GLView(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_context(0)
{
	m_context = SCUM::GLContext::create(window()->handle(), this);
	assert(m_context != 0);
}

SCUM_GLView::~SCUM_GLView()
{
	delete m_context;
}

void SCUM_GLView::refreshGL()
{
	m_context->refresh();
}

#if 0
void SCUM_Window::pushClip(const SCUM_Rect& clip)
{
#if SCUM_NO_GL
	SCUM::pushClip(clip);
#else
	GLfloat fv[4];
	glGetFloatv(GL_SCISSOR_BOX, fv);
	SCUM_Rect scissor(fv[0], fv[1], fv[0]+fv[2], fv[1]+fv[3]);
	m_clipStack.push_back(scissor);
	scissor = scissor & clip;
	// cout << "pushClip: " << scissor << "\n";
	glScissor(scissor);
#endif
}

void SCUM_Window::popClip()
{
#if SCUM_NO_GL
	SCUM::popClip();
#else
	SCUM_Rect scissor(m_clipStack.back());
	m_clipStack.pop_back();
	glScissor(scissor);
#endif
}
#endif // 0

// EOF
