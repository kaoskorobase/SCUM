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


#ifndef SCUM_COLOR_HH_INCLUDED
#define SCUM_COLOR_HH_INCLUDED

#include <stdint.h>
#include <iostream>

namespace SCUM
{
	static const double kRGBAScale = 1.0/255.0;
};

class SCUM_Color
{
public:
	union RGBA
	{
		uint32_t		u32;
		uint8_t			u8[4];
	};

	SCUM_Color()
	{
		rgba.u32 = 0;
	}
	SCUM_Color(float r, float g, float b, float a=1.0f)
	{
		rgba.u8[0] = (uint8_t)(255. * r);
		rgba.u8[1] = (uint8_t)(255. * g);
		rgba.u8[2] = (uint8_t)(255. * b);
		rgba.u8[3] = (uint8_t)(255. * a);
	}
	SCUM_Color(uint32_t packed)
	{
		rgba.u8[0] = (uint8_t)((packed >> 24) & 0xFF);
		rgba.u8[1] = (uint8_t)((packed >> 16) & 0xFF);
		rgba.u8[2] = (uint8_t)((packed >> 8) & 0xFF);
		rgba.u8[3] = (uint8_t)(packed & 0xFF);
	}
	SCUM_Color(const SCUM_Color& color)
	{
		rgba.u32 = color.rgba.u32;
	}

	uint32_t rgba32() const
	{
		return (uint32_t)((((((rgba.u8[0] << 8) | rgba.u8[1]) << 8) | rgba.u8[2]) << 8) | rgba.u8[3]);
	}

	uint8_t r8() const { return rgba.u8[0]; }
	uint8_t g8() const { return rgba.u8[1]; }
	uint8_t b8() const { return rgba.u8[2]; }
	uint8_t a8() const { return rgba.u8[3]; }

	float r32() const { return rgba.u8[0] * SCUM::kRGBAScale; }
	float g32() const { return rgba.u8[1] * SCUM::kRGBAScale; }
	float b32() const { return rgba.u8[2] * SCUM::kRGBAScale; }
	float a32() const { return rgba.u8[3] * SCUM::kRGBAScale; }

	bool isTransparent() const { return a8() == 0; }
	inline SCUM_Color scale(float alpha) const;
	inline SCUM_Color blend(const SCUM_Color& c, float blend=0.5) const;

	template <class T> void copy(T* dst) const
	{
		dst[0] = r32();
		dst[1] = g32();
		dst[2] = b32();
		dst[3] = a32();
	}

	RGBA		rgba;
};

inline SCUM_Color SCUM_Color::scale(float alpha) const
{
	return SCUM_Color(r32() * alpha, g32() * alpha, b32() * alpha, alpha);
}

inline SCUM_Color SCUM_Color::blend(const SCUM_Color& c, float blend) const
{
	float r = r32();
	float g = g32();
	float b = b32();
	float a = a32();

	return SCUM_Color(
		r + blend * (c.r32() - r),
		g + blend * (c.g32() - g),
		b + blend * (c.b32() - b),
		a + blend * (c.a32() - a)
		);
}

inline std::ostream& operator << (std::ostream& stream, const SCUM_Color& color)
{
	stream << "SCUM_Color("
		   << color.r32() << ", " << color.g32() << ", "
		   << color.b32() << ", " << color.a32() << ")";
	return stream;
}

#endif // SCUM_COLOR_HH_INCLUDED
