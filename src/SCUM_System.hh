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

	$Id: SCUM_System.hh,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#ifndef SCUM_SYSTEM_HH_INCLUDED
#define SCUM_SYSTEM_HH_INCLUDED

#include "SCUM_Color.hh"
#include "SCUM_Font.hh"
#include "SCUM_Geometry.hh"
#include "SCUM_Image.hh"
#include "SCUM_Timer.hh"

// namespace SCUM
// {
// 	class CairoContext
// 	{
// 	public:
// 		static CairoContext* create(SCUM_View* view);

// 		virtual SCUM_Rect bounds() = 0;
// 		virtual void setBounds(const SCUM_Rect& bounds) = 0;
// 		virtual void refresh() = 0;
// 		virtual void drawText(FontHandle* font, const char* str) = 0;
// 	};
// }; // SCUM

namespace SCUM
{
	enum ModMask
	{
		kModMaskShift	= (1 << 0),
		kModMaskControl	= (1 << 1),
		kModMaskCommand	= (1 << 2),
		kModMaskKeypad	= (1 << 3)
	};

	void init();
	SCUM_Size screenSize();
	double time();

	// actions
	void addTimer(SCUM_Timer* timer);
};

namespace SCUM
{
	enum LineStyle
	{
		kLineStyleSolid,
		kLineStyleDash,
		kLineStyleDot
	};

	enum LineCap
	{
		kLineCapFlat,
		kLineCapRound,
		kLineCapSquare
	};

	enum LineJoin
	{
		kLineJoinMiter,
		kLineJoinRound,
		kLineJoinBevel
	};

	// manage graphic context stack
	// includes: color, line [width, style, cap, join], font
	void GCSave();
	void GCRestore();
	
	void GCSetColor(const SCUM_Color& c);
	void GCSetLineWidth(float width);
	void GCSetLineStyle(LineStyle style);
	void GCSetLineCap(LineCap cap);
	void GCSetLineJoin(LineJoin join);
	void GCSetFont(FontHandle* font);
	inline void GCSetFont(const SCUM_Font& font);

	// drawing
	void GCDrawPoint(float x, float y);
	inline void GCDrawPoint(const SCUM_Point& p);
		
	void GCDrawLine(float x1, float y1, float x2, float y2);
	inline void GCDrawLine(const SCUM_Point& p1, const SCUM_Point& p2);

	void GCDrawRect(float x, float y, float w, float h);
	inline void GCDrawRect(const SCUM_Rect& r);
		
	void GCDrawBeveledRect(float x, float y, float w, float w, float bw, bool inout);
	inline void GCDrawBeveledRect(const SCUM_Rect& r, float bw, bool inout);
		
	void GCFillRect(float x, float y, float w, float h);
	inline void GCFillRect(const SCUM_Rect& r);

	void GCDrawPolygon(float x1, float y1, float x2, float y2, float x3, float y3);
	inline void GCDrawPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3);

	void GCFillPolygon(float x1, float y1, float x2, float y2, float x3, float y3);
	inline void GCFillPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3);
		
	void GCDrawArc(float x, float y, float w, float h, float a1, float a2);
	inline void GCDrawArc(const SCUM_Rect& bounds, float a1, float a2);
		
	void GCFillArc(float x, float y, float w, float h, float a1, float a2);
	inline void GCFillArc(const SCUM_Rect& bounds, float a1, float a2);
		
	void GCDrawText(float x, float y, const char* str);
	inline void GCDrawText(const SCUM_Point& pos, const char* str);

	void GCDrawImage(SCUM::ImageHandle* img, float x, float y, float w, float h);
	inline void GCDrawImage(const SCUM_Image& img, const SCUM_Rect& bounds);
	inline void GCDrawImage(const SCUM_Image& img, const SCUM_Point& pos);

	// clipping
	void GCInitClip();
	void GCSetClip(const SCUM_Rect& r);
	bool GCIsClipped(const SCUM_Rect& r);
	SCUM_Rect GCClippedRect(const SCUM_Rect& r);
};

inline void SCUM::GCSetFont(const SCUM_Font& font)
{
	if (font.isValid()) GCSetFont(const_cast<SCUM_Font&>(font).handle());
}

inline void SCUM::GCDrawPoint(const SCUM_Point& p)
{
	GCDrawPoint(p.x, p.y);
}

inline void SCUM::GCDrawLine(const SCUM_Point& p1, const SCUM_Point& p2)
{
	GCDrawLine(p1.x, p1.y, p2.x, p2.y);
}

inline void SCUM::GCDrawRect(const SCUM_Rect& r)
{
	GCDrawRect(r.x, r.y, r.w, r.h);
}

inline void SCUM::GCDrawBeveledRect(const SCUM_Rect& r, float bw, bool inout)
{
	GCDrawBeveledRect(r.x, r.y, r.w, r.h, bw, inout);
}

inline void SCUM::GCFillRect(const SCUM_Rect& r)
{
	GCFillRect(r.x, r.y, r.w, r.h);
}

inline void SCUM::GCDrawPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3)
{
	GCDrawPolygon(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

inline void SCUM::GCFillPolygon(const SCUM_Point& p1, const SCUM_Point& p2, const SCUM_Point& p3)
{
	GCFillPolygon(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

inline void SCUM::GCDrawArc(const SCUM_Rect& bounds, float a1, float a2)
{
	GCDrawArc(bounds.x, bounds.y, bounds.w, bounds.h, a1, a2);
}

inline void SCUM::GCFillArc(const SCUM_Rect& bounds, float a1, float a2)
{
	GCFillArc(bounds.x, bounds.y, bounds.w, bounds.h, a1, a2);
}

inline void SCUM::GCDrawText(const SCUM_Point& pos, const char* str)
{
	GCDrawText(pos.x, pos.y, str);
}

inline void SCUM::GCDrawImage(const SCUM_Image& img, const SCUM_Rect& bounds)
{
	GCDrawImage(const_cast<SCUM_Image&>(img).handle(), bounds.x, bounds.y, bounds.w, bounds.h);
}

inline void SCUM::GCDrawImage(const SCUM_Image& img, const SCUM_Point& pos)
{
	GCDrawImage(img, SCUM_Rect(pos, img.size()));
}

#endif // SCUM_SYSTEM_HH_INCLUDED
