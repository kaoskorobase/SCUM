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

	$Id: SCUM_ScrollView.cpp,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#include "SCUM_ScrollView.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"

using namespace SCUM;

// =====================================================================
// SCUM_ScrollView

SCUM_ScrollView::SCUM_ScrollView(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_scrollStep(1, 1),
	  m_thumbSize(15),
	  m_hThumb(true),
	  m_vThumb(true)
{
}

bool SCUM_ScrollView::mouseDown(int state, const SCUM_Point& where)
{
	if (m_hThumb && (where.y > m_viewPortBounds.maxY())) {
		if (m_hThumbBounds.contains(where)) {
			m_mouseMoveStart = where.x;
			m_mouseMoveOrient = Horizontal;
			return true;
		}
		return false;
	}
	if (m_vThumb && (where.x > m_viewPortBounds.maxX())) {
		if (m_vThumbBounds.contains(where)) {
			m_mouseMoveStart = where.y;
			m_mouseMoveOrient = Vertical;
			return true;
		}
		return false;
	}
	return mouseDownInContent(state, where);
}

void SCUM_ScrollView::mouseMove(int state, const SCUM_Point& where)
{
	if (hasMouse()) {
		if (m_mouseMoveOrient == Horizontal) {
			scrollBy(SCUM_Point(where.x - m_mouseMoveStart, 0));
			m_mouseMoveStart = where.x;
		} else {
			scrollBy(SCUM_Point(0, where.y - m_mouseMoveStart));
			m_mouseMoveStart = where.y;
		}
		refresh();
	} else {
		mouseMoveInContent(state, where);
	}
}

void SCUM_ScrollView::mouseUp(int state, const SCUM_Point& where)
{
	if (!hasMouse()) mouseUpInContent(state, where);
}

void SCUM_ScrollView::scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
	if (state == 0) {
		// FIXME: better scaling
		scrollBy(SCUM_Point(delta.x * m_scrollStep.x, delta.y * m_scrollStep.y));
		refresh();
	} else {
		scrollWheelInContent(state, where, delta);
	}
}

void SCUM_ScrollView::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::x) {
		setHScrollRatio(floatValue(slot));
		refresh();
	} else if (key == SCUM_Symbol::y) {
		setVScrollRatio(floatValue(slot));
		refresh();
	} else if (equal(key, "thumbSize")) {
		m_thumbSize = max(5, intValue(slot));
		updateLayout();
	} else if (equal(key, "hThumb")) {
		m_hThumb = boolValue(slot);
		updateLayout();
	} else if (equal(key, "vThumb")) {
		m_vThumb = boolValue(slot);
		updateLayout();
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_ScrollView::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::x) {
		setFloatValue(slot, scrollRatio().x);
	} else if (key == SCUM_Symbol::y) {
		setFloatValue(slot, scrollRatio().y);
	} else if (equal(key, "thumbSize")) {
		setIntValue(slot, m_thumbSize);
	} else if (equal(key, "hThumb")) {
		setBoolValue(slot, m_hThumb);
	} else if (equal(key, "vThumb")) {
		setBoolValue(slot, m_vThumb);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

void SCUM_ScrollView::setHScrollRatio(double ratio)
{
	m_scrollRatio.x = clip(ratio, 0., 1.);
	updateHThumbBounds();
}

void SCUM_ScrollView::setVScrollRatio(double ratio)
{
	m_scrollRatio.y = clip(ratio, 0., 1.);
	updateVThumbBounds();
}

void SCUM_ScrollView::setHScrollOffset(double offset)
{
	double range = hRange();
	if (range > 0.) {
// 		double value = floor(offset / m_scrollStep.x + 0.5) * m_scrollStep.x;
		setHScrollRatio(offset / range);
	} else {
		setHScrollRatio(0.);
	}
}

void SCUM_ScrollView::setVScrollOffset(double offset)
{
	double range = vRange();
	if (range > 0.) {
// 		double value = floor(offset / m_scrollStep.y + 0.5) * m_scrollStep.y;
		setVScrollRatio(offset / range);
	} else {
		setVScrollRatio(0.);
	}
}

void SCUM_ScrollView::scrollBy(const SCUM_Point& delta)
{
	if (delta.x != 0.) {
		setHScrollOffset(hScrollOffset() + delta.x);
// 			floor((hScrollOffset() + delta.x) / m_scrollStep.x + 0.5) * m_scrollStep.x
// 			);
	}
	if (delta.y != 0.) {
		setVScrollOffset(vScrollOffset() + delta.y);
// 			floor((vScrollOffset() + delta.y) / m_scrollStep.y + 0.5) * m_scrollStep.y
// 			);
	}
}

void SCUM_ScrollView::scrollCenter(const SCUM_Point& point)
{
	// TODO: implement me
}

void SCUM_ScrollView::scrollVisible(const SCUM_Rect& rect, const SCUM_Point& padding)
{
	SCUM_Point delta;

	if ((rect.x < m_viewPortBounds.x) && (m_viewPortBounds.maxX() > rect.maxX())) {
		delta.x = rect.x - m_viewPortBounds.x + padding.x;
	} else if ((rect.maxX() < m_viewPortBounds.maxX()) && (m_viewPortBounds.x > rect.x)) {
		delta.x = rect.maxX() - m_viewPortBounds.maxX() + padding.x;
	}
		
	if ((rect.y < m_viewPortBounds.y) && (m_viewPortBounds.maxY() > rect.maxY())) {
		delta.y = rect.y - m_viewPortBounds.y - padding.y;
	} else if ((rect.maxY() > m_viewPortBounds.maxY()) && (m_viewPortBounds.y < rect.y)) {
		delta.y = rect.maxY() - m_viewPortBounds.maxY() - padding.y;
	}

	scrollBy(delta);
}

void SCUM_ScrollView::drawView()
{
	const SCUM_Rect r(bounds());

// 	updateThumbBounds();

	saveGCState();

	setColor(bgColor());
	fillRect(r);
	drawBeveledRect(r, 1, true);

	if (m_hThumb) drawBeveledRect(m_hThumbBounds, 1, false);
	if (m_vThumb) drawBeveledRect(m_vThumbBounds, 1, false);

	pushClip(m_viewPortBounds);
	drawContent();
	popClip();

	restoreGCState();
}

void SCUM_ScrollView::drawFocus()
{
	drawContentFocus();
}

void SCUM_ScrollView::drawContent()
{
}

void SCUM_ScrollView::drawContentFocus()
{
	SCUM_View::drawFocus(m_viewPortBounds);
}

bool SCUM_ScrollView::mouseDownInContent(int, const SCUM_Point&)
{
	return false;
}

void SCUM_ScrollView::mouseMoveInContent(int, const SCUM_Point&)
{
}

void SCUM_ScrollView::mouseUpInContent(int, const SCUM_Point&)
{
}

void SCUM_ScrollView::scrollWheelInContent(int, const SCUM_Point&, const SCUM_Point&)
{
}

SCUM_Size SCUM_ScrollView::preferredSize()
{
	SCUM_Size size = preferredViewPortSize();
	size.w += 1.f;
	size.h += 1.f;
	if (m_vThumb) size.w += m_thumbSize;
	if (m_hThumb) size.h += m_thumbSize;
	return size;
}

SCUM_Size SCUM_ScrollView::preferredViewPortSize()
{
	return SCUM_Size(0, 0);
}

void SCUM_ScrollView::boundsChanged(const SCUM_Rect& bounds)
{
	SCUM_View::boundsChanged(bounds);
	m_viewPortBounds = bounds.inset(1);
	if (m_vThumb) m_viewPortBounds.w -= m_thumbSize;
	if (m_hThumb) m_viewPortBounds.h -= m_thumbSize;
	updateThumbBounds();
}

void SCUM_ScrollView::setContentSize(const SCUM_Size& size)
{
	m_contentBounds.w = size.w;
	m_contentBounds.h = size.h;
	updateThumbBounds();
	refresh();
}

void SCUM_ScrollView::updateThumbBounds()
{
	updateHThumbBounds();
	updateVThumbBounds();
}

void SCUM_ScrollView::updateHThumbBounds()
{
	if (m_hThumb) {
		m_hThumbBounds.w = hRatio() * m_viewPortBounds.w;
	} else {
		m_hThumbBounds.w = m_viewPortBounds.w;
	}

	m_hThumbBounds.h = m_thumbSize;
	m_hThumbBounds.x = m_viewPortBounds.x + hRange() * m_scrollRatio.x;
	m_hThumbBounds.y = bounds().maxY() - m_thumbSize - 1;

	m_contentBounds.x = m_viewPortBounds.x - m_scrollRatio.x * hRange() / hRatio();
}

void SCUM_ScrollView::updateVThumbBounds()
{
	if (m_vThumb) {
		m_vThumbBounds.h = vRatio() * m_viewPortBounds.h;
// 		printf("vRatio %f %f %f\n", vRatio(), m_vThumbBounds.h / m_viewPortBounds.h, vRange());
	} else {
		m_vThumbBounds.h = m_viewPortBounds.h;
	}

	m_vThumbBounds.w = m_thumbSize;
	m_vThumbBounds.x = bounds().maxX() - m_thumbSize - 1;
	m_vThumbBounds.y = m_viewPortBounds.y + vRange() * m_scrollRatio.y;

	m_contentBounds.y = m_viewPortBounds.y - m_scrollRatio.y * vRange() / vRatio();
}

// EOF
