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

	$Id: SCUM_Geometry.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_GEOMETRY_HH_INCLUDED
#define SCUM_GEOMETRY_HH_INCLUDED

#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>

namespace SCUM
{
	enum
	{
		AlignNW = 1,
		AlignN,
		AlignNE,
		AlignW,
		AlignC,
		AlignE,
		AlignSW,
		AlignS,
		AlignSE
	};

	inline int checkAlign(int align);

	template <class T> T min(T a, T b)
	{
		return (a > b) ? b : a;
	}

	template <class T> T max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template <class T> T clip(T x, T min, T max)
	{
		if (x < min) x = min;
		else if (x > max) x = max;
		return x;
	}
};

class SCUM_Point
{
public:
	SCUM_Point()
		: x(0.0f), y(0.0f)
	{ }
	SCUM_Point(float xy)
		: x(xy), y(xy)
	{ }
	SCUM_Point(float x1, float y1)
		: x(x1), y(y1)
	{ }
	SCUM_Point(const SCUM_Point& point)
		: x(point.x), y(point.y)
	{ }

	inline SCUM_Point min(const SCUM_Point& p) const;
	inline SCUM_Point min(float f) const;
	inline SCUM_Point max(const SCUM_Point& p) const;
	inline SCUM_Point max(float f) const;

	template <class T> void copy(T* dst) const
	{
		dst[0] = x; dst[1] = y;
	}

	float x, y;
};

typedef std::vector<SCUM_Point> SCUM_PointArray;

class SCUM_Size
{
public:
	SCUM_Size()
		: w(0.0f), h(0.0f)
	{ }
	SCUM_Size(float wh)
		: w(wh), h(wh)
	{ }
	SCUM_Size(float w1, float h1)
		: w(w1), h(h1)
	{ }
	SCUM_Size(const SCUM_Point& p)
		: w(fabs(p.x)), h(fabs(p.y))
	{ }
	SCUM_Size(const SCUM_Size& size)
		: w(size.w), h(size.h)
	{ }

	bool notEmpty() const { return (w > 0.0f) && (h > 0.0f); }
	bool isEmpty() const { return !notEmpty(); }

	inline SCUM_Size max(const SCUM_Size& s) const;
	inline SCUM_Size padded(const SCUM_Point& padding) const;
	inline SCUM_Size padded(float padding) const;
	inline SCUM_Point layout(const SCUM_Size& box, int align) const;

	template <class T> void copy(T* dst) const
	{
		dst[0] = w; dst[1] = h;
	}

	float w, h;
};

inline SCUM_Point SCUM_Point::min(const SCUM_Point& p) const
{
	return SCUM_Point(SCUM::min(x, p.x), SCUM::min(y, p.y));
}

inline SCUM_Point SCUM_Point::min(float f) const
{
	return SCUM_Point(SCUM::min(x, f), SCUM::min(y, f));
}

inline SCUM_Point SCUM_Point::max(const SCUM_Point& p) const
{
	return SCUM_Point(SCUM::max(x, p.x), SCUM::max(y, p.y));
}

inline SCUM_Point SCUM_Point::max(float f) const
{
	return SCUM_Point(SCUM::max(x, f), SCUM::max(y, f));
}

inline bool operator == (const SCUM_Point& p1, const SCUM_Point& p2)
{
	return (p1.x == p2.x) && (p1.y == p2.y);
}

inline bool operator != (const SCUM_Point& p1, const SCUM_Point& p2)
{
	return !(p1 == p2);
}

inline SCUM_Point operator + (const SCUM_Point& p1, const SCUM_Point& p2)
{
	return SCUM_Point(p1.x + p2.x, p1.y + p2.y);
}

inline SCUM_Point operator + (const SCUM_Point& p, float f)
{
	return SCUM_Point(p.x + f, p.y + f);
}

inline SCUM_Point operator + (const SCUM_Point& p, const SCUM_Size& s)
{
	return SCUM_Point(p.x + s.w, p.y + s.h);
}

inline SCUM_Point operator - (const SCUM_Point& p1, const SCUM_Point& p2)
{
	return SCUM_Point(p1.x - p2.x, p1.y - p2.y);
}

inline SCUM_Point operator - (const SCUM_Point& p, float f)
{
	return SCUM_Point(p.x - f, p.y - f);
}

inline SCUM_Point operator * (const SCUM_Point& p, float f)
{
	return SCUM_Point(p.x * f, p.y * f);
}

inline SCUM_Size SCUM_Size::max(const SCUM_Size& s) const
{
	return SCUM_Size(std::max(w, s.w), std::max(h, s.h));
}

inline SCUM_Size SCUM_Size::padded(const SCUM_Point& padding) const
{
	return SCUM_Size(w + 2.0f * padding.x, h + 2.0f * padding.y);
}

inline SCUM_Size SCUM_Size::padded(float padding) const
{
	register float padding2 = 2.0f * padding;
	return SCUM_Size(w + padding2, h + padding2);
}

inline SCUM_Point SCUM_Size::layout(const SCUM_Size& box, int align) const
{
	SCUM_Point delta;

	switch (align) {
		case SCUM::AlignNW:
			break;
		case SCUM::AlignN:
			delta.x = std::max(0.0f, 0.5f * (w - box.w));
			break;
		case SCUM::AlignNE:
			delta.x = std::max(0.0f, w - box.w);
			break;
		case SCUM::AlignW:
			delta.y = std::max(0.0f, 0.5f * (h - box.h));
			break;
		case SCUM::AlignC:
			delta.x = std::max(0.0f, 0.5f * (w - box.w));
			delta.y = std::max(0.0f, 0.5f * (h - box.h));
			break;
		case SCUM::AlignE:
			delta.x = std::max(0.0f, w - box.w);
			delta.y = std::max(0.0f, 0.5f * (h - box.h));
			break;
		case SCUM::AlignSW:
			delta.y = std::max(0.0f, h - box.h);
			break;
		case SCUM::AlignS:
			delta.x = std::max(0.0f, 0.5f * (w - box.w));
			delta.y = std::max(0.0f, h - box.h);
			break;
		case SCUM::AlignSE:
			delta.x = std::max(0.0f, w - box.w);
			delta.y = std::max(0.0f, h - box.h);
			break;
	}

	return delta;
}

inline bool operator == (const SCUM_Size& s1, const SCUM_Size& s2)
{
	return (s1.w == s2.w) && (s1.h == s2.h);
}

inline bool operator != (const SCUM_Size& s1, const SCUM_Size& s2)
{
	return !(s1 == s2);
}

inline SCUM_Size operator + (const SCUM_Size& s, float f)
{
	return SCUM_Size(s.w + f, s.h + f);
}

class SCUM_Rect
{
public:
	SCUM_Rect()
		: x(0.0f), y(0.0f), w(0.0f), h(0.0f)
	{ }
	SCUM_Rect(float inX, float inY, float width, float height)
		: x(inX), y(inY), w(width), h(height)
	{ }
	SCUM_Rect(const SCUM_Point& origin, const SCUM_Point& extent)
		: x(origin.x), y(origin.y), w(extent.x-x), h(extent.y-y)
	{ }
	SCUM_Rect(const SCUM_Point& origin, const SCUM_Size& size)
		: x(origin.x), y(origin.y), w(size.w), h(size.h)
	{ }
	SCUM_Rect(const SCUM_Rect& r)
		: x(r.x), y(r.y), w(r.w), h(r.h)
	{ }

	float minX() const { return x; }
	float minY() const { return y; }
	float maxX() const { return x+w; }
	float maxY() const { return y+h; }

	SCUM_Point origin() const { return SCUM_Point(x, y); }
	SCUM_Point extent() const { return SCUM_Point(x+w, y+h); }
	SCUM_Size size() const { return SCUM_Size(w, h); }

	bool notEmpty() const { return (w > 0.0f) && (h > 0.0f); }
	bool isEmpty() const { return !notEmpty(); }

	inline bool contains(const SCUM_Point& point) const;
	inline bool intersects(const SCUM_Rect& rect) const;
	inline SCUM_Rect translated(const SCUM_Point& delta) const;
	inline SCUM_Rect inset(const SCUM_Point& delta) const;
	inline SCUM_Rect inset(float delta) const;
	inline SCUM_Rect flipped() const;

	template <class T> void copy(T* dst) const
	{
		dst[0] = x;
		dst[1] = y;
		dst[2] = w;
		dst[3] = h;
	}

	float x, y, w, h;
};

inline bool SCUM_Rect::contains(const SCUM_Point& point) const
{
	return (point.x >= minX()) && (point.x <= maxX())
		&& (point.y >= minY()) && (point.y <= maxY());
}

inline bool SCUM_Rect::intersects(const SCUM_Rect& r) const
{
	return !((maxX() <= r.minX()) ||
			 (r.maxX() <= minX()) ||
			 (maxY() <= r.minY()) ||
			 (r.maxY() <= minY()));
}

inline SCUM_Rect SCUM_Rect::translated(const SCUM_Point& delta) const
{
	return SCUM_Rect(x+delta.y, y+delta.y, w, h);
}

inline SCUM_Rect SCUM_Rect::inset(const SCUM_Point& delta) const
{
	SCUM_Rect r(*this);
	r.x += delta.x;
	r.y += delta.y;
	r.w = SCUM::max(0.0f, r.w - 2.0f * delta.x);
	r.h = SCUM::max(0.0f, r.h - 2.0f * delta.y);
	return r;
}

inline SCUM_Rect SCUM_Rect::inset(float delta) const
{
	SCUM_Rect r(*this);
	r.x += delta;
	r.y += delta;
	float delta2 = 2.0f * delta;
	r.w = SCUM::max(0.0f, r.w - delta2);
	r.h = SCUM::max(0.0f, r.h - delta2);
	return r;
}

inline SCUM_Rect SCUM_Rect::flipped() const
{
	return SCUM_Rect(minX(), h - maxY(), w, h);
}

inline bool operator == (const SCUM_Rect& r1, const SCUM_Rect& r2)
{
	return (r1.x == r2.x) && (r1.y == r2.y) && (r1.w == r2.w) && (r1.h == r2.h);
}

inline bool operator != (const SCUM_Rect& r1, const SCUM_Rect& r2)
{
	return !(r1 == r2);
}

// inline SCUM_Rect operator & (const SCUM_Rect& r1, const SCUM_Rect& r2)
// {
// 	// intersection
// 	if (r1.intersects(r2)) {
// 		float x11 = r1.minX();
// 		float y11 = r1.minY();
// 		return SCUM_Rect(r1.origin.max(r2.origin),
// 						 r1.extent.min(r2.extent));
// 	}
	
// 	return SCUM_Rect();
// }

// inline SCUM_Rect operator | (SCUM_Rect& r1, const SCUM_Rect& r2)
// {
// 	// union
// 	bool e1 = r1.isEmpty();
// 	bool e2 = r2.isEmpty();

// 	if (e1 && e2) {
// 		return SCUM_Rect();
// 	} else if (e1) {
// 		return r2;
// 	} else if (e2) {
// 		return r1;
// 	}
	
// 	return SCUM_Rect(r1.origin.min(r2.origin),
// 					 r1.extent.max(r2.extent));
// }

inline std::ostream& operator << (std::ostream& stream, const SCUM_Point& p)
{
	stream << "(" << p.x << ", " << p.y << ")";
	return stream;
}

inline std::ostream& operator << (std::ostream& stream, const SCUM_Size& s)
{
	stream << "(" << s.w << " x " << s.h << ")";
	return stream;
}

inline std::ostream& operator << (std::ostream& stream, const SCUM_Rect& r)
{
	stream << "[" << r.x << ", " << r.y << ", " << r.w << ", " << r.h << "]";
	return stream;
}

inline int SCUM::checkAlign(int align)
{
	return SCUM::clip<int>(align, SCUM::AlignNW, SCUM::AlignSE);
}

#endif // SCUM_GEOMETRY_HH_INCLUDED
