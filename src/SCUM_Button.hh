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

	$Id: SCUM_Button.hh,v 1.2 2004/08/04 11:48:25 steve Exp $
*/


#ifndef SCUM_BUTTON_HH_INCLUDED
#define SCUM_BUTTON_HH_INCLUDED

#include "SCUM_View.hh"

#include <stdint.h>
#include <vector>

// =====================================================================
// SCUM_Toggle

class SCUM_Toggle : public SCUM_View
{
public:
	SCUM_Toggle(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView(const SCUM_Rect& damage);
	
	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void mouseUp(int state, const SCUM_Point& where);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

protected:
	bool setValue(bool value, bool send);
	bool toggleValue(bool send);

private:
	unsigned	m_value : 1;
	unsigned	m_pushed : 1;
};

// =====================================================================
// SCUM_Bang

class SCUM_Bang : public SCUM_View
{
public:
	SCUM_Bang(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView(const SCUM_Rect& damage);

	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual void animate();

protected:
	void bang(bool send);

private:
	double		m_flashTime;
	double		m_timeStamp;
	unsigned	m_active : 1;
};

// =====================================================================
// SCUM_Button

class SCUM_Button : public SCUM_View
{
public:
	struct State
	{
		std::string		text;
		SCUM_Color		fgColor;
		SCUM_Color		bgColor;
	};

	typedef std::vector<State> StateArray;

public:
	SCUM_Button(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView(const SCUM_Rect& damage);
	virtual void drawFocus(const SCUM_Rect& damage);

	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void mouseUp(int state, const SCUM_Point& where);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual SCUM_Size preferredSize();

private:
	void stateValue(PyrSlot* slot, StateArray& array);
	bool setValue(int value, bool send);

private:
	SCUM_Font			m_font;
	SCUM_Point			m_padding;
	StateArray			m_states;
	int					m_value;
	uint8_t				m_textAlign;
	uint8_t				m_pushed;
};

// =====================================================================
// SCUM_Choice

class SCUM_Menu;

class SCUM_Choice : public SCUM_View
{
public:
	SCUM_Choice(SCUM_Container* parent, PyrObject* obj);
	virtual ~SCUM_Choice();

	virtual void drawView(const SCUM_Rect& damage);

	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);
	virtual void contextMenu(int state, const SCUM_Point& where);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual SCUM_Size preferredSize();

private:
	bool setValue(int value, bool send);

private:
	SCUM_Font					m_font;
	SCUM_Point					m_padding;
	std::vector<std::string>	m_states;
	int							m_value;
	SCUM_Menu*					m_menu;
	uint8_t						m_textAlign;
};

#endif // SCUM_BUTTON_HH_INCLUDED
