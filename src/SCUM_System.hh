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

	$Id: SCUM_System.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_SYSTEM_HH_INCLUDED
#define SCUM_SYSTEM_HH_INCLUDED

#include "SCUM_Color.hh"
#include "SCUM_Font.hh"
#include "SCUM_Geometry.hh"

class SCUM_Timer;

namespace SCUM
{
	enum
	{
		ModShift		= (1 << 0),
		ModControl		= (1 << 1),
		ModCommand		= (1 << 2),
		ModKeypad		= (1 << 3)
	};

	enum
	{
		LineSolid,
		LineDash,
		LineDot
	};

	enum
	{
		Horizontal,
		Vertical
	};

	enum
	{
		CondReadable = 1,
		CondWritable = 2
	};

	void init();
	SCUM_Size screenSize();
	double time();

	// actions
	void addTimer(SCUM_Timer* timer);

	// manage clip stack
	void pushClip(float x, float y, float w, float h);
	inline void pushClip(const SCUM_Rect& r);
	void pushNoClip();
	void popClip();
	bool isClipped(const SCUM_Rect& r);
	SCUM_Rect clippedRect(const SCUM_Rect& r);

	// manage graphic context stack
	// includes: color, line style, line width
	void saveGCState();
	void restoreGCState();

	void setFont(SCUM_FontHandle* fontHandle);
	inline void setFont(SCUM_Font& font);

	void setColor(const SCUM_Color& c);
	void setLineStyle(float width, int style=SCUM::LineSolid);

	void drawPoint(float x, float y);
	inline void drawPoint(const SCUM_Point& p);

	void drawLine(float x1, float y1, float x2, float y2);
	inline void drawLine(const SCUM_Point& p1, const SCUM_Point& p2) { drawLine(p1.x, p1.y, p2.x, p2.y); }

	void drawRect(float x, float y, float w, float h);
	inline void drawRect(const SCUM_Rect& r);

// 	void drawBorderRect(int type, float x, float y, float w, float h, float bw);
// 	inline void drawBorderRect(int type, const SCUM_Rect& r, float bw);
	void drawBeveledRect(float x, float y, float w, float w, float bw, bool inout);
	inline void drawBeveledRect(const SCUM_Rect& r, float bw, bool inout);

	void fillRect(float x, float y, float w, float h);
	inline void fillRect(const SCUM_Rect& r);

	void fillRoundedRect(float x, float y, float w, float h, float radius);
	inline void fillRoundedRect(const SCUM_Rect& r, float radius);

	void drawPolygon(float x1, float y1, float x2, float y2, float x3, float y3);
	inline void drawPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3);

	void fillPolygon(float x1, float y1, float x2, float y2, float x3, float y3);
	inline void fillPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3);

	void drawArc(float x, float y, float w, float h, float a1, float a2);
	inline void drawArc(const SCUM_Rect& bounds, float a1, float a2);

	void fillArc(float x, float y, float w, float h, float a1, float a2);
	inline void fillArc(const SCUM_Rect& bounds, float a1, float a2);

	void drawText(float x, float y, const char* str);
	inline void drawText(const SCUM_Point& pos, const char* str);
};

class SCUM_Timer
{
public:
	SCUM_Timer(float timeout)
		: m_valid(true), m_timeout(timeout)
	{ }

	bool isValid() const { return m_valid; }
	void remove() { m_valid = false; }

	float timeout() const { return m_timeout; }
	void setTimeout(float timeout) { m_timeout = timeout; }

	virtual void operator () () = 0;

private:
	bool				m_valid;
	float				m_timeout;
};

template <class T> class SCUM_MemTimer : public SCUM_Timer
{
public:
	typedef void (T::*Callback)(SCUM_Timer* timer);

	SCUM_MemTimer(float timeout, Callback cb, T* obj)
		: SCUM_Timer(timeout),
		  m_cb(cb), m_obj(obj)
	{ }

	virtual void operator () () { (m_obj->*m_cb)(this); }

private:
	Callback			m_cb;
	T*					m_obj;
};

inline void SCUM::pushClip(const SCUM_Rect& r)
{
	pushClip(r.x, r.y, r.w, r.h);
}

inline void SCUM::setFont(SCUM_Font& font)
{
	if (font.isValid()) setFont(font.handle());
}

inline void SCUM::drawPoint(const SCUM_Point& p)
{
	SCUM::drawPoint(p.x, p.y);
}

inline void SCUM::drawRect(const SCUM_Rect& r)
{
	SCUM::drawRect(r.x, r.y, r.w, r.h);
}

// inline void SCUM::drawBorderRect(int type, const SCUM_Rect& r, float bw)
// {
// 	SCUM::drawBorderRect(type, r.x, r.y, r.w, r.h, bw);
// }

// inline void SCUM::drawBeveledRect(float x, float y, float w, float h, float bw, bool inout)
// {
// 	SCUM::drawBorderRect(inout ? BorderSunken : BorderRaised, x, y, w, h, bw);
// }

inline void SCUM::drawBeveledRect(const SCUM_Rect& r, float bw, bool inout)
{
	SCUM::drawBeveledRect(r.x, r.y, r.w, r.h, bw, inout);
}

inline void SCUM::fillRect(const SCUM_Rect& r)
{
	SCUM::fillRect(r.x, r.y, r.w, r.h);
}

inline void SCUM::fillRoundedRect(const SCUM_Rect& r, float radius)
{
	SCUM::fillRoundedRect(r.x, r.y, r.w, r.h, radius);
}

inline void SCUM::drawPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3)
{
	SCUM::drawPolygon(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

inline void SCUM::fillPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3)
{
	SCUM::fillPolygon(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

inline void SCUM::drawArc(const SCUM_Rect& bounds, float a1, float a2)
{
	SCUM::drawArc(bounds.x, bounds.y, bounds.w, bounds.h, a1, a2);
}

inline void SCUM::fillArc(const SCUM_Rect& bounds, float a1, float a2)
{
	SCUM::fillArc(bounds.x, bounds.y, bounds.w, bounds.h, a1, a2);
}

inline void SCUM::drawText(const SCUM_Point& pos, const char* str)
{
	drawText(pos.x, pos.y, str);
}

#endif // SCUM_SYSTEM_HH_INCLUDED
