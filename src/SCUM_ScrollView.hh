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

	$Id$
*/


#ifndef SCUM_SCROLLVIEW_HH_INCLUDED
#define SCUM_SCROLLVIEW_HH_INCLUDED

#include "SCUM_View.hh"
#include <stdint.h>

class SCUM_ScrollView : public SCUM_View
{
public:
	SCUM_ScrollView(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);

	const SCUM_Rect& viewPortBounds() const { return m_viewPortBounds; }
	const SCUM_Rect& contentBounds() const { return m_contentBounds; }
	inline const SCUM_Point& scrollRatio() const;
	inline double hScrollOffset() const;
	inline double vScrollOffset() const;
	inline SCUM_Point scrollOffset() const;

	inline double hRange() const;
	inline double vRange() const;
	inline double hRatio() const;
	inline double vRatio() const;
	
	virtual bool mouseDown(int, const SCUM_Point&);
	virtual void mouseMove(int, const SCUM_Point&);
	virtual void mouseUp(int, const SCUM_Point&);
	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	void setHScrollRatio(double ratio);
	void setVScrollRatio(double ratio);
	inline void setScrollRatio(const SCUM_Point& ratio);
	void setHScrollOffset(double offset);
	void setVScrollOffset(double offset);
	inline void setScrollOffset(const SCUM_Point& offset);
	void scrollBy(const SCUM_Point& delta);
	void scrollCenter(const SCUM_Point& point);
	void scrollVisible(const SCUM_Rect& rect, const SCUM_Point& padding);

protected:
	virtual void drawView(const SCUM_Rect& damage);
	virtual void drawFocus(const SCUM_Rect& damage);
	virtual void drawContent(const SCUM_Rect& damage);
	virtual void drawContentFocus(const SCUM_Rect& damage);

	virtual bool mouseDownInContent(int, const SCUM_Point&);
	virtual void mouseMoveInContent(int, const SCUM_Point&);
	virtual void mouseUpInContent(int, const SCUM_Point&);
	virtual void scrollWheelInContent(int, const SCUM_Point&, const SCUM_Point&);

	virtual SCUM_Size getMinSize();
	virtual SCUM_Size preferredViewPortSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);
	void setContentSize(const SCUM_Size& size);

	SCUM_Point	m_scrollStep;

private:
	void updateThumbBounds();
	void updateHThumbBounds();
	void updateVThumbBounds();

private:
	int			m_thumbSize;
	SCUM_Rect	m_viewPortBounds;
	SCUM_Rect	m_contentBounds;
	SCUM_Rect	m_vThumbBounds;
	SCUM_Rect	m_hThumbBounds;
	SCUM_Point	m_scrollRatio;
	float		m_mouseMoveStart;
	uint8_t		m_mouseMoveOrient;
	uint8_t		m_hThumb;
	uint8_t		m_vThumb;
};

inline const SCUM_Point& SCUM_ScrollView::scrollRatio() const
{
	return m_scrollRatio;
}

inline double SCUM_ScrollView::hScrollOffset() const
{
	return hRange() * m_scrollRatio.x;
}

inline double SCUM_ScrollView::vScrollOffset() const
{
	return vRange() * m_scrollRatio.y;
}

inline SCUM_Point SCUM_ScrollView::scrollOffset() const
{
	return SCUM_Point(hScrollOffset(), vScrollOffset());
}

inline void SCUM_ScrollView::setScrollRatio(const SCUM_Point& ratio)
{
	setHScrollRatio(ratio.x);
	setVScrollRatio(ratio.y);
}

inline void SCUM_ScrollView::setScrollOffset(const SCUM_Point& offset)
{
	setHScrollOffset(offset.x);
	setVScrollOffset(offset.y);
}

inline double SCUM_ScrollView::hRange() const
{
	return m_viewPortBounds.w - m_hThumbBounds.w;
}

inline double SCUM_ScrollView::vRange() const
{
	return m_viewPortBounds.h - m_vThumbBounds.h;
}

inline double SCUM_ScrollView::hRatio() const
{
	return (m_contentBounds.w > 0.)
		? SCUM::min(1., (double)m_viewPortBounds.w / (double)m_contentBounds.w)
		: 1.;
}

inline double SCUM_ScrollView::vRatio() const
{
	return (m_contentBounds.h > 0.)
		? SCUM::min(1., (double)m_viewPortBounds.h / (double)m_contentBounds.h)
		: 1.;
}

#endif // SCUM_SCROLLVIEW_HH_INCLUDED
