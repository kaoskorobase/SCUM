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

	$Id: SCUM_Font.hh,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#ifndef SCUM_FONT_HH_INCLUDED
#define SCUM_FONT_HH_INCLUDED

#include "SCUM_Geometry.hh"

#include <map>
#include <string>

// class SCUM_FontHandle
// {
// public:
// 	SCUM_FontHandle();
// 	virtual ~SCUM_FontHandle();

// 	virtual float height() = 0;
// 	virtual float ascent() = 0;
// 	virtual float descent() = 0;
// 	virtual SCUM_Size measure(const char* str) = 0;
// };

// namespace SCUM
// {
// 	SCUM_FontHandle* makeFontHandle(const char* name, float size);
// };

namespace SCUM
{
	class FontHandle
	{
	public:
		static FontHandle* create(const char* name, float size);

		virtual float height() = 0;
		virtual float ascent() = 0;
		virtual float descent() = 0;

		virtual SCUM_Size measure(const char* str) = 0;
	};
};

class SCUM_Font
{
public:
	SCUM_Font(const char* name="Arial", float size=12.f);
	SCUM_Font(const SCUM_Font& font);
	~SCUM_Font();

	SCUM::FontHandle* handle() { return m_handle; }
	const std::string& name() const { return m_name; }
	float size() const { return m_size; }
	bool isValid() const { return m_handle != 0; }

	float height() { return m_handle ? m_handle->height() : 0; }
	float ascent() { return m_handle ? m_handle->ascent() : 0; }
	float descent() { return m_handle ? m_handle->descent() : 0; }

	void setFont(const char* name, float size=0.f);
	inline void setFont(const std::string& name, float size=0.f);

	inline SCUM_Size measure(const char* str);
	inline SCUM_Size measure(const std::string& str);

	void draw(const SCUM_Rect& bounds, const char* str, int align);
	inline void draw(const SCUM_Rect& bounds, const std::string& str, int align);

	inline SCUM_Font& operator = (const SCUM_Font& font);

private:
	std::string			m_name;
	float				m_size;
	SCUM::FontHandle*	m_handle;
};

inline void SCUM_Font::setFont(const std::string& name, float size)
{
	setFont(name.c_str(), size);
}

inline SCUM_Size SCUM_Font::measure(const char* str)
{
	return m_handle ? m_handle->measure(str) : SCUM_Size();
}

inline SCUM_Size SCUM_Font::measure(const std::string& str)
{
	return measure(str.c_str());
}

inline void SCUM_Font::draw(const SCUM_Rect& bounds, const std::string& str, int align)
{
	draw(bounds, str.c_str(), align);
}

inline SCUM_Font& SCUM_Font::operator = (const SCUM_Font& font)
{
	setFont(font.name(), font.size());
	return *this;
}

#endif // SCUM_FONT_HH_INCLUDED
