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


#ifndef SCUM_FONT_HH_INCLUDED
#define SCUM_FONT_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Handle.hh"

// =====================================================================
// SCUM_Font

struct SCUM_TextExtents
{
	SCUM_Size	size;
};

struct SCUM_FontExtents
{
	SCUM_FontExtents()
		: ascent(0.f), descent(0.f), height(0.f)
	{ }

	float		ascent;
	float		descent;
	float		height;
};

class SCUM_Font
{
public:
	SCUM_Font(const char* name="Helvetica", double size=12.f);
	SCUM_Font(const SCUM_Font& font);

	inline int id() const;
	inline int size() const;
	inline bool isValid() const;

	inline const SCUM_FontExtents& extents() const;
	SCUM_TextExtents measure(const char* str) const;

	void draw(const SCUM_Point& pos, const char* str);
	void draw(const SCUM_Rect& bounds, SCUM::Align align, const char* str);

	void drawGL(const SCUM_Point& pos, const char* str);
	void drawGL(const SCUM_Rect& bounds, SCUM::Align align, const char* str);

private:
	int					m_id;
	int					m_size;
	SCUM_FontExtents	m_extents;
};

// =====================================================================
// SCUM_Text

class SCUM_Text
{
public:
	SCUM_Text();
	SCUM_Text(const char* text, size_t size);
	SCUM_Text(const char* text);
	SCUM_Text(const SCUM_Text& other);
	~SCUM_Text();

	inline const char* text() const;
	inline const SCUM_Font& font() const;
	inline const SCUM_TextExtents& extents() const;

	void setText(const char* text, size_t size);
	void setText(const char* text);
	void setFont(const SCUM_Font& font);
	SCUM_Text& operator = (const SCUM_Text& other);

	inline void draw(const SCUM_Point& pos);
	inline void draw(const SCUM_Rect& bounds, SCUM::Align align);

	inline void drawGL(const SCUM_Point& pos);
	inline void drawGL(const SCUM_Rect& bounds, SCUM::Align align);

private:
	void changed();
	void copyText(const char* text, size_t size);

private:
	char*				m_text;
	SCUM_Font			m_font;
	SCUM_TextExtents	m_extents;
};

// =====================================================================
// SCUM_Font (inline functions)

inline int SCUM_Font::id() const
{
	return m_id;
}

inline int SCUM_Font::size() const
{
	return m_size;
}

inline bool SCUM_Font::isValid() const
{
	return m_id != -1;
}

inline const SCUM_FontExtents& SCUM_Font::extents() const
{
	return m_extents;
}

// =====================================================================
// SCUM_Text (inline functions)

inline const char* SCUM_Text::text() const
{
	return m_text ? m_text : "";
}

inline const SCUM_Font& SCUM_Text::font() const
{
	return m_font;
}

inline const SCUM_TextExtents& SCUM_Text::extents() const
{
	return m_extents;
}

inline void SCUM_Text::draw(const SCUM_Point& pos)
{
	if (m_text) m_font.draw(pos, m_text);
}

inline void SCUM_Text::draw(const SCUM_Rect& bounds, SCUM::Align align)
{
	if (m_text) m_font.draw(bounds, align, m_text);
}

inline void SCUM_Text::drawGL(const SCUM_Point& pos)
{
	if (m_text) m_font.drawGL(pos, m_text);
}

inline void SCUM_Text::drawGL(const SCUM_Rect& bounds, SCUM::Align align)
{
	if (m_text) m_font.drawGL(bounds, align, m_text);
}

#endif // SCUM_FONT_HH_INCLUDED
