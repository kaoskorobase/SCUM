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


#include "SCUM_GC.hh"
#include "SCUM_Util.hh"

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <stdexcept>

namespace SCUM
{
	class GC
	{
	public:
		enum { kMaxDepth = 64 };

		struct State
		{
			inline void pushClip(const SCUM_Rect& r)
			{
				SCUM_ASSERT(clipped == 0);
				clipped = 1;
				fl_push_clip((int)r.x, (int)r.y, (int)r.w, (int)r.h);
			}
			inline void pushNoClip()
			{
				SCUM_ASSERT(clipped == 0);
				clipped = 1;
				fl_push_no_clip();
			}
			inline void popClip()
			{
				SCUM_ASSERT(clipped == 1);
				clipped = 0;
				fl_pop_clip();
			}

			inline void applyColor()
			{
				fl_color(color);
			}
			inline void applyLineStyle()
			{
				fl_line_style(
					lineStyle|lineCap|lineJoin,
					lineWidth,
					0
					);
			}
			inline void apply()
			{
				applyColor();
				applyLineStyle();
			}

			Fl_Color		color;
			int				lineWidth;
			int				lineStyle;
			int				lineCap;
			int				lineJoin;
			int				clipped;
		};

		GC();

		void validate();
		void save();
		void restore();

		State	state;

	private:
		State*	m_ptr;
		State*	m_limit;
		State	m_stack[kMaxDepth];
	};

	static GC gGC;
};

// =====================================================================
// SCUM::GC

SCUM::GC::GC()
{
	state.clipped = 0;
	m_ptr = m_limit = m_stack + kMaxDepth;
}

void SCUM::GC::save()
{
	if (m_ptr == m_stack) {
		throw std::runtime_error("GC stack overflow");
	}
	*(--m_ptr) = state;
}

void SCUM::GC::restore()
{
	if (m_ptr == m_limit) {
		throw std::runtime_error("GC stack underflow");
	}
	state = *(m_ptr++);
	state.apply();
}

void SCUM::GC::validate()
{
	if (state.clipped) {
		throw std::runtime_error("invalid GC clip stack depth");
	}
	if (m_ptr != m_limit) {
		throw std::runtime_error("invalid GC state stack depth");
	}
}

// =====================================================================
// SCUM GC interface

void SCUM::GCInit()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);
	fl_font(FL_HELVETICA, 12);
}

void SCUM::GCSave()
{
	gGC.save();
}

void SCUM::GCRestore()
{
	if (gGC.state.clipped) gGC.state.popClip();
	gGC.restore();
}

void SCUM::GCSetColor(const SCUM_Color& c)
{
	gGC.state.color = fl_rgb_color(c.r8(), c.g8(), c.b8());
	gGC.state.applyColor();
}

void SCUM::GCSetLineWidth(float width)
{
	gGC.state.lineWidth = (int)width;
	gGC.state.applyLineStyle();
}

void SCUM::GCSetLineStyle(SCUM::LineStyle style)
{
	switch (style) {
		case SCUM::kLineStyleSolid: gGC.state.lineStyle = FL_SOLID; break;
		case SCUM::kLineStyleDash: gGC.state.lineStyle = FL_DASH; break;
		case SCUM::kLineStyleDot: gGC.state.lineStyle = FL_DOT; break;
	}
	gGC.state.applyLineStyle();
}

void SCUM::GCSetLineCap(SCUM::LineCap cap)
{
	switch (cap) {
		case SCUM::kLineCapFlat: gGC.state.lineCap = FL_CAP_FLAT; break;
		case SCUM::kLineCapRound: gGC.state.lineCap = FL_CAP_ROUND; break;
		case SCUM::kLineCapSquare: gGC.state.lineCap = FL_CAP_SQUARE; break;
	}
	gGC.state.applyLineStyle();
}

void SCUM::GCSetLineJoin(SCUM::LineJoin join)
{
	switch (join) {
		case SCUM::kLineJoinMiter: gGC.state.lineJoin = FL_JOIN_MITER; break;
		case SCUM::kLineJoinRound: gGC.state.lineJoin = FL_JOIN_ROUND; break;
		case SCUM::kLineJoinBevel: gGC.state.lineJoin = FL_JOIN_BEVEL; break;
	}
	gGC.state.applyLineStyle();
}

void SCUM::GCDrawPoint(float x, float y)
{
	fl_point((int)x, (int)y);
}

void SCUM::GCDrawLine(float x1, float y1, float x2, float y2)
{
	fl_line((int)x1, (int)y1, (int)x2, (int)y2);
}

void SCUM::GCDrawRect(float x, float y, float w, float h)
{
	fl_rect((int)x, (int)y, (int)w, (int)h);
}

namespace SCUM
{
	inline void GCDrawBeveledRectNW(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_yxline(x1++, y2--, y1++, x2--);
	}

	inline void GCDrawBeveledRectSE(int x1, int y1, int x2, int y2, int bw)
	{
		while (bw--) fl_xyline(x1++, y2--, x2--, y1++);
	}
};

void SCUM::GCDrawBeveledRect(float xf, float yf, float wf, float hf, float bwf, bool inout)
{
	Fl_Color bc = gGC.state.color;
	SCUM_Color sbc = SCUM_Color((uint32_t)bc & 0xFFFFFF00);
	Fl_Color lc = (Fl_Color)(sbc.blend(SCUM_Color(1, 1, 1), 0.3).rgba32() & 0xFFFFFF00);
	Fl_Color dc = (Fl_Color)(sbc.blend(SCUM_Color(0, 0, 0), 0.3).rgba32() & 0xFFFFFF00);
// 	Fl_Color lc = (Fl_Color)(SCUM_Color(1, 1, 1).rgba32() & 0xFFFFFF00);
// 	Fl_Color dc = (Fl_Color)(SCUM_Color(0, 0, 0).rgba32() & 0xFFFFFF00);

	int x1 = (int)xf;
	int y1 = (int)yf;
	int x2 = (int)(xf+wf) - 1;
	int y2 = (int)(yf+hf) - 1;
	int bw = (int)bwf;

	// draw SE
	if (inout) fl_color(lc);
	else fl_color(dc);
	GCDrawBeveledRectSE(x1, y1, x2, y2, bw);

	// draw NW (overlaps SE)
	if (inout) fl_color(dc);
	else fl_color(lc);
	GCDrawBeveledRectNW(x1, y1, x2, y2, bw);

	// restore color
	fl_color(bc);
}

void SCUM::GCFillRect(float x, float y, float w, float h)
{
	fl_rectf((int)x, (int)y, (int)w, (int)h);
}

void SCUM::GCDrawPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_loop((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::GCFillPolygon(float x1, float y1, float x2, float y2, float x3, float y3)
{
	fl_polygon((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3);
}

void SCUM::GCDrawArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_arc((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::GCFillArc(float x, float y, float w, float h, float a1, float a2)
{
	fl_pie((int)x, (int)y, (int)w, (int)h, a1, a2);
}

void SCUM::GCInitClip()
{
	if (gGC.state.clipped) gGC.state.popClip();
	gGC.state.pushNoClip();
}

void SCUM::GCSetClip(const SCUM_Rect& r)
{
	if (gGC.state.clipped) gGC.state.popClip();
	gGC.state.pushClip(r);
}

bool SCUM::GCIsClipped(const SCUM_Rect& r)
{
	return !fl_not_clipped((int)r.x, (int)r.y, (int)r.w, (int)r.h);
}

SCUM_Rect SCUM::GCClippedRect(const SCUM_Rect& r)
{
	int x, y, w, h;
	if (fl_clip_box((int)r.x, (int)r.y, (int)r.w, (int)r.h, x, y, w, h))
		return SCUM_Rect(x, y, w, h);
	return r;
}

// EOF
