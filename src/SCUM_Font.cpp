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


#include "SCUM_FLUtil.hh"
#include "SCUM_Font.hh"
#include "SCUM_System.hh"
#include "SCUM_Util.hh"

#include <FL/fl_draw.H>
#include <FL/gl.h>

using namespace SCUM;

// =====================================================================
// SCUM_Font

namespace SCUM
{
	struct FLFontDesc
	{
		const char* name;
		int id;
	};

	static FLFontDesc gFontTable[] = {
		{ "Helvetica", FL_HELVETICA },
		{ "Arial", FL_HELVETICA },
		{ "sans", FL_HELVETICA },
		{ "Times", FL_TIMES },
		{ "serif", FL_TIMES },
		{ "Courier", FL_COURIER },
		{ "Monaco", FL_COURIER },
		{ "monospace", FL_COURIER },
		{ "Symbol", FL_SYMBOL },
		{ "ZapfDingbats", FL_ZAPF_DINGBATS }
	};
	static const int kNumFonts = sizeof(gFontTable)/sizeof(FLFontDesc);

	static bool getFontID(const char* name, int& id)
	{
		bool found = false;
		
		const char* end = strchr(name, '-');
		size_t len = end ? end - name : strlen(name);

		for (int i=0; i < kNumFonts; i++) {
			if (strncmp(name, gFontTable[i].name, len) == 0) {
				id = gFontTable[i].id;
				found = true;
				break;
			}
		}
		
		if (!found) return false;

		while (true) {
			name = strchr(name, '-');
			if (!name) break;
			name++;
			if (strncmp(name, "Bold", 4) == 0) {
				id |= FL_BOLD;
			} else if (strncmp(name, "Italic", 6) == 0) {
				id |= FL_ITALIC;
			} else if (strncmp(name, "Oblique", 7) == 0) {
				id |= FL_ITALIC;
			}
		}
		
		return true;
	}

	typedef void (*SetFontFunction)(Fl_Font, int);

	static struct
	{
		SetFontFunction	func;
		Fl_Font			id;
		int				size;
	} gSaveFont;

	static void SetFont(Fl_Font id, int size)
	{
		fl_font(id, size);
	}
	static void SetFontGL(Fl_Font id, int size)
	{
		gl_font(id, size);
	}

	static inline void setFont(SetFontFunction func, const SCUM_Font& font)
	{
		SCUM_ASSERT(gSaveFont.func == 0);
		gSaveFont.func = func;
		gSaveFont.id = (Fl_Font)font.id();
		gSaveFont.size = font.size();
		(*func)(gSaveFont.id, gSaveFont.size);
	}
	static inline void restoreFont()
	{
		SCUM_ASSERT(gSaveFont.func != 0);
		(*gSaveFont.func)(gSaveFont.id, gSaveFont.size);
		gSaveFont.func = 0;
	}
}

SCUM_Font::SCUM_Font(const char* name, double size)
	: m_id(-1),
	  m_size(0)
{
	if (getFontID(name, m_id)) {
		m_size = (int)SCUM::max(0., size);
		setFont(&SetFont, *this);
		m_extents.height = fl_height();
		m_extents.descent = fl_descent();
		m_extents.ascent = m_extents.height - m_extents.descent;
		restoreFont();
	}
}

SCUM_Font::SCUM_Font(const SCUM_Font& other)
	: m_id(other.m_id),
	  m_size(other.m_size),
	  m_extents(other.m_extents)
{
}

SCUM_TextExtents SCUM_Font::measure(const char* str) const
{
	SCUM_TextExtents res;
	if (isValid()) {
		int w, h;
		setFont(&SetFont, *this);
		fl_measure(str, w, h, 0);
		res.size.w = w;
		res.size.h = h;
		restoreFont();
	}
	return res;
}

void SCUM_Font::draw(const SCUM_Point& pos, const char* str)
{
	if (isValid()) {
		setFont(&SetFont, *this);
		fl_draw(str, pos.xi(), pos.yi());
		restoreFont();
	}
}

void SCUM_Font::draw(const SCUM_Rect& bounds, SCUM::Align align, const char* str)
{
	if (isValid()) {
		setFont(&SetFont, *this);
#if 0
		SCUM_Point pos(
			bounds.origin() +
			bounds.size().layout(extents.size, align)
			);
// 		pos.y += this->extents().ascent;
		fl_draw(
			str,
			pos.xi(), pos.yi(),
			extents.size.wi(), extents.size.hi(),
			(Fl_Align)(FL_ALIGN_TOP|FL_ALIGN_LEFT),
			0, 0
			);
#endif
		fl_draw(
			str,
			bounds.xi(), bounds.yi(),
			bounds.wi(), bounds.hi(),
			convert(align),
			0, 0
			);
		restoreFont();
	}
}

void SCUM_Font::drawGL(const SCUM_Point& pos, const char* str)
{
	if (isValid()) {
		setFont(&SetFontGL, *this);
		gl_draw(str, pos.x, pos.y);
		restoreFont();
	}
}

void SCUM_Font::drawGL(const SCUM_Rect& bounds, SCUM::Align align, const char* str)
{
	if (isValid()) {
		setFont(&SetFontGL, *this);
		gl_draw(
			str,
			bounds.xi(), bounds.yi(),
			bounds.wi(), bounds.hi(),
			convert(align)
			);
		restoreFont();
	}
}

// =====================================================================
// SCUM_Text

SCUM_Text::SCUM_Text()
	: m_text(0)
{
}

SCUM_Text::SCUM_Text(const char* text, size_t size)
	: m_text(0)
{
	copyText(text, size);
}

SCUM_Text::SCUM_Text(const char* text)
	: m_text(0)
{
	copyText(text, strlen(text));
}

SCUM_Text::SCUM_Text(const SCUM_Text& other)
	: m_text(0),
	  m_font(other.m_font),
	  m_extents(other.m_extents)
{
	copyText(other.m_text, other.m_text ? strlen(other.m_text) : 0);
}

SCUM_Text::~SCUM_Text()
{
	copyText(0, 0);
}

void SCUM_Text::setText(const char* text, size_t size)
{
	copyText(text, size);
	changed();
}

void SCUM_Text::setText(const char* text)
{
	copyText(text, strlen(text));
	changed();
}

void SCUM_Text::setFont(const SCUM_Font& font)
{
	m_font = font;
	changed();
}

SCUM_Text& SCUM_Text::operator = (const SCUM_Text& other)
{
	copyText(other.m_text, strlen(other.m_text));
	m_font = other.m_font;
	m_extents = other.m_extents;
}

void SCUM_Text::changed()
{
	if (m_text) m_extents = m_font.measure(m_text);
}

void SCUM_Text::copyText(const char* text, size_t size)
{
	delete [] m_text;
	if (text && (size > 0)) {
		m_text = new char[size+1];
		strncpy(m_text, text, size);
		m_text[size] = '\0';
	} else {
		m_text = 0;
	}
}

#if 0
// GL fonts
#include <fontconfig/fontconfig.h>
#include <FTGLTextureFont.h>
#include <FTGLOutlineFont.h>
#include <FTGLPixmapFont.h>
#include <FTGLBitmapFont.h>
#include <FTGLPolygonFont.h>

#include <math.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;

#define SCUM_TEXTURE_FONT 0
#define SCUM_POLYGON_FONT 1
#define SCUM_PIXMAP_FONT  2
#define SCUM_BITMAP_FONT  3

#define SCUM_FONT_TYPE 2

#define SCUM_FONT_RENDER_1(POINT, STRING) \
		glPushAttrib(GL_ENABLE_BIT); \
		glEnable(GL_TEXTURE_2D); \
		glMatrixMode(GL_MODELVIEW); \
		glPushMatrix(); \
		glTranslate(POINT); \
		m_handle->Render(STRING); \
		glPopMatrix(); \
		glPopAttrib()


#define SCUM_FONT_RENDER_2(POINT, STRING) \
		glRasterPos(POINT); \
		m_handle->Render(STRING)

#if SCUM_FONT_TYPE == SCUM_TEXTURE_FONT
# define SCUM_FONT_CLASS FTGLTextureFont
# define SCUM_FONT_RENDER(p, s) SCUM_FONT_RENDER_1(p, s)
#endif

#if SCUM_FONT_TYPE == SCUM_POLYGON_FONT
# define SCUM_FONT_CLASS FTGLPolygonFont
# define SCUM_FONT_RENDER(p, s) SCUM_FONT_RENDER_1(p, s)
#endif

#if SCUM_FONT_TYPE == SCUM_PIXMAP_FONT
# define SCUM_FONT_CLASS FTGLPixmapFont
# define SCUM_FONT_RENDER(p, s) SCUM_FONT_RENDER_2(p, s)
#endif

#if SCUM_FONT_TYPE == SCUM_BITMAP_FONT
# define SCUM_FONT_CLASS FTGLBitmapFont
# define SCUM_FONT_RENDER(p, s) SCUM_FONT_RENDER_2(p, s)
#endif

class SCUM_FontManager
{
public:
	SCUM_FontManager();

	static SCUM_FontManager& instance();

	FTFont* getFont(const char* name);
	void initGL();

private:
	FcPattern* matchName(const char* name);

	typedef std::map<std::string,FTFont*> Cache;
	typedef Cache::iterator CacheIter;

	Cache m_cache;
};

SCUM_Font::SCUM_Font(const char* name, size_t size)
	: m_name(name), m_size(size), m_handle(0)
{
	char buffer[strlen(name)+32];
	sprintf(buffer, "%s-%d", name, size);
	m_handle = SCUM_FontManager::instance().getFont(buffer);
}

SCUM_Font::SCUM_Font(const SCUM_Font& font)
	: m_name(font.m_name), m_size(font.m_size), m_handle(font.m_handle)
{
}

bool SCUM_Font::isValid() const
{
	return m_handle != 0;
}

SCUM_Size SCUM_Font::sizeOf(const char* str)
{
	if (m_handle) {
		float h = m_handle->Ascender() - m_handle->Descender();
		float w = 0.0f;
		float x0, x1, f;
		m_handle->BBox(str, x0, f, f, x1, f, f);
		// TODO: get rid of initial lead?
		w = x1; //- x0;
		return SCUM_Size(ceilf(w), ceilf(h));
	}
	return SCUM_Size();
}

void SCUM_Font::draw(const char* str, const SCUM_Rect& bounds, int align)
{
	if (m_handle) {
		SCUM_Point delta = bounds.origin + bounds.size().layout(sizeOf(str), align);
		delta.y = delta.y - m_handle->Descender();
		SCUM_FONT_RENDER(delta, str);
	}
}

SCUM_Font SCUM_Font::parse(const char* spec)
{
	char* sizeStr = strchr(spec, ':');
	std::string name(spec, sizeStr ? sizeStr - spec : strlen(spec));
	size_t size = sizeStr ? abs(atoi(sizeStr+1)) : 0;
	if (size == 0) size = 12;
	cout << "SCUM_Font::parse: " << name << " " << size << "\n";
	return SCUM_Font(name.c_str(), size);
}

// =====================================================================
// SCUM_FontManager

SCUM_FontManager::SCUM_FontManager()
{
	FcInit();
}

SCUM_FontManager& SCUM_FontManager::instance()
{
	static SCUM_FontManager s_instance;
	return s_instance;
}

FTFont* SCUM_FontManager::getFont(const char* name)
{
	FcPattern* match = matchName(name);
	if (!match)	return 0;

	char* keyStr = (char*)FcNameUnparse(match);
	std::string key(keyStr);
	free(keyStr);

	CacheIter iter = m_cache.find(key);
	if (iter != m_cache.end()) {
#ifndef NDEBUG
		cout << "SCUM_FontManager::getFont: using cached font\n";
#endif
		FcPatternDestroy(match);
		return iter->second;
	}
	
	FcResult res;
	const char* fileName;
	int size;
	double dpi;

	res = FcPatternGetString(match, FC_FILE, 0, (FcChar8**)&fileName);
	if (res != FcResultMatch) {
		FcPatternDestroy(match);
		return 0;
	}

	res = FcPatternGetInteger(match, FC_SIZE, 0, &size);
	if (res != FcResultMatch) {
		FcPatternDestroy(match);
		return 0;
	}

	res = FcPatternGetDouble(match, FC_DPI, 0, &dpi);
	if (res != FcResultMatch) {
		FcPatternDestroy(match);
		return 0;
	}

#ifndef NDEBUG
	printf("SCUM_FontManager::getFont: fileName %s size %d dpi %f\n",
		   fileName, size, dpi);
#endif

	FTFont* font = new SCUM_FONT_CLASS(fileName);
	FcPatternDestroy(match);

	if (!(font
		  && (font->Error() == FT_Err_Ok)
		  && font->FaceSize(size, (unsigned int)dpi)))
	{
		delete font;
		return 0;
	}
	
	m_cache[key] = font;

	return font;
}

FcPattern* SCUM_FontManager::matchName(const char* name)
{
	FcConfig* conf = FcConfigGetCurrent();
	FcPattern* pat = FcNameParse((const FcChar8*)name);

	FcDefaultSubstitute(pat);

	if (!FcConfigSubstitute(conf, pat, FcMatchPattern)) {
#ifndef NDEBUG
		printf("SCUM_FontManager::matchName: FcConfigSubstitute failed\n");
#endif
		FcPatternDestroy(pat);
		return 0;
	}

	FcResult res;
	FcPattern* match = FcFontMatch(conf, pat, &res);
	//if (res != FcResultMatch) {
	//printf("FcFontMatch failed %d\n", res);
	//FcPatternDestroy(match);
	//FcPatternDestroy(pat);
	//return 0;
	//}

	FcPatternDestroy(pat);

	return match;
}
#endif // 0

// EOF
