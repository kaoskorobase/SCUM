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

	$Id: SCUM_GL.cpp,v 1.3 2004/08/15 14:42:23 steve Exp $
*/


#include "SCUM_GC.hh"
#include "SCUM_GL.hh"
#include "SCUM_Util.hh"
#include "SCUM_Window.hh"

using namespace SCUM;

// =====================================================================
// SCUM_GLContext

SCUM_GLContext::SCUM_GLContext(SCUM_GLView* view)
	: Fl_Gl_Window(0, 0, 10, 10),
	  m_view(view)
{
	// TODO: pass flags into constructor?
	mode(FL_RGB|FL_DOUBLE|FL_DEPTH|FL_ALPHA|FL_STENCIL);
	view->window()->asGroup()->add(this);
	end();
}

SCUM_GLContext::~SCUM_GLContext()
{
	SCUM_ASSERT_PTR(parent());
	parent()->remove(this);
	m_view->releaseContext();
}

void SCUM_GLContext::refresh()
{
	redraw();
}

int SCUM_GLContext::handle(int evt)
{
	return (evt == FL_SHOW) || (evt == FL_HIDE) ?
		Fl_Gl_Window::handle(evt) : 0;
}

void SCUM_GLContext::draw()
{
	if (!valid()) {
// 		SCUM_DEBUG_PRINT("initGL: %d %d %d %d\n",
// 						 x(), y(), w(), h());
		m_view->initGL();
	}
	m_view->drawGL();
}

// =====================================================================
// SCUM_View

SCUM_GLView::SCUM_GLView(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj)
{
	m_context = new SCUM_GLContext(this);
}

SCUM_GLView::~SCUM_GLView()
{
	delete m_context;
}

void SCUM_GLView::refresh(const SCUM_Rect& damage)
{
	SCUM_View::refresh(damage);
	m_context->refresh();
}

void SCUM_GLView::refresh()
{
	refresh(bounds());
}

void SCUM_GLView::drawView(const SCUM_Rect& damage)
{
	GCSetColor(bgColor());
	GCFillRect(bounds());
	if (m_border != kBorderNone)
		GCDrawBeveledRect(bounds(), 1, m_border == kBorderIn);
}

void SCUM_GLView::setBounds(const SCUM_Rect& bounds)
{
	SCUM_View::setBounds(bounds);
	m_context->setBounds(bounds.inset(m_padding + (m_border != kBorderNone)));
}

SCUM_Size SCUM_GLView::getMinSize()
{
	return SCUM_View::getMinSize().padded(m_padding + (m_border != kBorderNone));
}

void SCUM_GLView::initGL()
{
}

void SCUM_GLView::drawGL()
{
}

void SCUM_GLView::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "visible")) {
		bool vis0 = isVisible();
		SCUM_View::setProperty(key, slot);
		bool vis1 = isVisible();
		if (vis0 != vis1) {
			if (vis1) m_context->show();
			else m_context->hide();
		}
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

// EOF
