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


#ifndef SCUM_GC_HH_INCLUDED
#define SCUM_GC_HH_INCLUDED

#include "SCUM_Color.hh"
#include "SCUM_Geometry.hh"

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

    void GCInit();

    // manage graphic context stack
    // includes: color, line [width, style, cap, join]
    void GCSave();
    void GCRestore();
	
    void GCSetColor(const SCUM_Color& c);
    void GCSetLineWidth(float width);
    void GCSetLineStyle(LineStyle style);
    void GCSetLineCap(LineCap cap);
    void GCSetLineJoin(LineJoin join);

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
		
    // 	void GCDrawImage(SCUM::ImageHandle* img, float x, float y, float w, float h);
    // 	inline void GCDrawImage(const SCUM_Image& img, const SCUM_Rect& bounds);
    // 	inline void GCDrawImage(const SCUM_Image& img, const SCUM_Point& pos);

    // clipping
    void GCInitClip();
    void GCSetClip(const SCUM_Rect& r);
    bool GCIsClipped(const SCUM_Rect& r);
    SCUM_Rect GCClippedRect(const SCUM_Rect& r);
};

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

// inline void SCUM::GCDrawImage(const SCUM_Image& img, const SCUM_Rect& bounds)
// {
// 	GCDrawImage(const_cast<SCUM_Image&>(img).handle(), bounds.x, bounds.y, bounds.w, bounds.h);
// }

// inline void SCUM::GCDrawImage(const SCUM_Image& img, const SCUM_Point& pos)
// {
// 	GCDrawImage(img, SCUM_Rect(pos, img.size()));
// }

#endif // SCUM_GC_HH_INCLUDED
