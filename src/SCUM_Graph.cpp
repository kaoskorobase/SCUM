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

	$Id: SCUM_Graph.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_Graph.hh"
#include "SCUM_System.hh"

#include <assert.h>

using namespace SCUM;

// =====================================================================
// SCUM_Scope

int getScopeBuf(uint32 index, SndBuf *buf, bool& didChange);

SCUM_Scope::SCUM_Scope(SCUM_Container* parent, PyrObject* obj)
	: SCUM_ScrollView(parent, obj),
	  m_context(0),
	  m_bufNum(-1),
	  m_zoom(1.f, 1.f),
	  m_style(kStyleSeparate)
{
	m_context = SCUM::GLContext::create(this);
	memset(&m_buf, 0, sizeof(SndBuf));
// 	layout().minSize = SCUM_Size(200, 100);
	startAnimation();
}

SCUM_Scope::~SCUM_Scope()
{
	free(m_buf.data);
	delete m_context;
}

void SCUM_Scope::scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
// 	if (state & ModShift) {
// 		m_zoom.x = max(1.0f, m_zoom.x + delta.x);
// 		m_zoom.y = max(1.0f, m_zoom.y + delta.y);
// 	} else {
// 		scrollByS
// 	}
	SCUM_ScrollView::scrollWheel(state, where, delta);
}

void SCUM_Scope::initGL()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_LINE_SMOOTH);
	glClearColor(bgColor());
}

void SCUM_Scope::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// shamelessly stolen from jmc's scope.

void SCUM_Scope::drawGL()
{
	bool didChange;
	int err = getScopeBuf(m_bufNum, &m_buf, didChange);
	if (err) return;

	switch (m_style) {
		case kStyleSeparate: drawSeparate(); break;
		case kStyleOverlay: drawOverlay(); break;
		case kStyleXY: drawXY(); break;
	}
}

void SCUM_Scope::drawSeparate()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float* data = m_buf.data;
	if (!data) return;

	int samples = m_buf.samples;
	SCUM_Rect bounds(m_context->bounds().flipped());
	float width = bounds.w;
	int channels = m_buf.channels;
	float chanHeight = bounds.h / channels;
	float chanHeight2 = 0.5f * chanHeight;

	SCUM_Point zoom(samples / (channels * width * m_zoom.x), chanHeight);
	SCUM_Point scroll(scrollOffset());

	glColor(fgColor());

	for (int j=0; j < channels; j++) {
		SCUM_Rect chanRect(0, chanHeight * j + chanHeight2, width, chanHeight);
		float fframe = scroll.x;
		int iframe = (int)floorf(fframe);
		int isample = 0;
		float val = data[isample + j];

		for (int i=0; (i < width) && (isample < samples); i++) {
			float ymin, ymax;
			ymin = ymax = val;
			float nextfframe = fframe + zoom.x;
			int nextiframe = (int)floorf(nextfframe);
			int nscan = nextiframe - iframe;

			for (int k=0; k < nscan; k++) {
				val = data[isample + j];
				if (val < ymin) ymin = val;
				else if (val > ymax) ymax = val;
				isample += channels;
			}

			fframe = nextfframe;
			iframe = nextiframe;

			SCUM_Rect srect;
			srect.x = i;
			srect.y = chanRect.y + ymin * chanHeight2;
			srect.w = 1.0f;
			srect.h = (ymax - ymin) * chanHeight2;

			glRectf(srect.x, srect.y, srect.x+srect.w, srect.y+srect.h);
		}
	}
}

void SCUM_Scope::drawOverlay()
{
}

void SCUM_Scope::drawXY()
{
	float* data = m_buf.data;
	if (!data) return;
	
	int samples = m_buf.samples;

	// draw scope.
	SCUM_Rect r(m_context->bounds().flipped());
	int channels = min(m_buf.channels, 16);
	float height = r.h - 2.;
	float height2 = 0.5 * height;
	float width = r.w - 2.;
	float width2 = 0.5 * width;
	float yScale = m_zoom.y * height2;
	float xScale = m_zoom.x * width2;
	float xoff = r.x + width2 + 1.;
	float yoff = r.y + height2 + 1.;
	
	glColor(bgColor());
	glRect(r);
	glColor(fgColor());

	for (int k=0, j=0; j < channels; k++, j+=2)
	{
// 		CGContextSetRGBStrokeColor(cgc, mWaveColors[k].red, mWaveColors[k].green, mWaveColors[k].blue, mWaveColors[k].alpha);
		float x = xoff + data[j] * xScale;
		float y = yoff - data[j+1] * yScale;

		glBegin(GL_LINES);
		for (int i=channels; i<samples; i+=channels) {
			x = xoff + data[i+j] * xScale;
			y = yoff - data[i+j+1] * yScale;
			glVertex2f(x, y);
		}
		glEnd();
	}
}

// SCUM_Size SCUM_Scope::preferredSize()
// {
// }

void SCUM_Scope::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "xZoom")) {
		m_zoom.x = max(1., floatValue(slot));
		updateContentSize();
	} else if (equal(key, "yZoom")) {
		m_zoom.y = max(1., floatValue(slot));
		updateContentSize();
	} else if (equal(key, "bufnum")) {
		m_bufNum = intValue(slot);
		updateContentSize();
	} else if (equal(key, "style")) {
		m_style = intValue(slot);
		m_context->refresh();
	} else {
		SCUM_ScrollView::setProperty(key, slot);
	}
}

void SCUM_Scope::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "xZoom")) {
		setFloatValue(slot, m_zoom.x);
	} else if (equal(key, "yZoom")) {
		setFloatValue(slot, m_zoom.y);
	} else if (equal(key, "bufnum")) {
		setIntValue(slot, m_bufNum);
	} else if (equal(key, "style")) {
		setIntValue(slot, m_style);
	} else {
		SCUM_ScrollView::getProperty(key, slot);
	}
}

void SCUM_Scope::animate()
{
	bool didChange = false;
	getScopeBuf(m_bufNum, &m_buf, didChange);
	if (didChange) updateContentSize();
}

void SCUM_Scope::refresh()
{
	SCUM_ScrollView::refresh();
	m_context->refresh();
}

void SCUM_Scope::boundsChanged(const SCUM_Rect& bounds)
{
	SCUM_ScrollView::boundsChanged(bounds);
	m_context->setBounds(viewPortBounds());
}

void SCUM_Scope::updateContentSize()
{
	SCUM_Size size(viewPortBounds().w * m_zoom.x, viewPortBounds().h);

	if (contentBounds().size() != size) {
		setContentSize(size);
	}

	m_context->refresh();
}

// EOF
