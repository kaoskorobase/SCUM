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

	$Id: SCUM_Slider.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_SLIDER_HH_INCLUDED
#define SCUM_SLIDER_HH_INCLUDED

#include "SCUM_View.hh"
#include <vector>

// =====================================================================
// SCUM_Slider

class SCUM_Slider : public SCUM_View
{
public:
	SCUM_Slider(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView();
	virtual void drawFocus();

	virtual bool mouseDown(int, const SCUM_Point&);
	virtual void mouseMove(int, const SCUM_Point&);
	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

protected:
	virtual SCUM_Size preferredSize();

	SCUM_Rect thumbRect();
	bool setValue(double value, bool send);
	double valueFromPoint(const SCUM_Point& p);

private:
	double		m_value;
	double		m_step;
	SCUM_Rect	m_thumbRect;
	int			m_thumbSize;
	bool		m_steady;
	double		m_steadyOffset;
};

// class SCUM_FillSlider : public SCUM_View
// {
// public:
// 	SCUM_FillSlider(SCUM_Container* parent, PyrObject* obj);

// 	virtual void drawView();
// 	virtual void drawFocus();

// 	virtual bool mouseDown(int, const SCUM_Point&);
// 	virtual void mouseMove(int, const SCUM_Point&);
// 	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

// 	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
// 	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

// protected:
// 	void updateThumbRect();
// 	bool setValue(double value, bool send);
// 	double valueFromPoint(const SCUM_Point& p);

// private:
// 	double		m_value;
// 	double		m_step;
// 	SCUM_Rect	m_thumbRect;
// 	int			m_thumbSize;
// 	bool		m_steady;
// 	double		m_steadyOffset;
// };

// =====================================================================
// SCUM_Pad

class SCUM_Pad : public SCUM_View
{
public:
	SCUM_Pad(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView();

	virtual bool mouseDown(int, const SCUM_Point&);
	virtual void mouseMove(int, const SCUM_Point&);
	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual SCUM_Size preferredSize();

protected:
	bool setValue(double ix, double iy, bool send);
	void valueFromPoint(const SCUM_Point& p, double& ox, double& oy);

private:
	double		m_x;
	double		m_y;
	double		m_xStep;
	double		m_yStep;
	bool		m_steady;
	SCUM_Point	m_steadyOffset;
};

#if 0
class SCUM_Knob : public SCUM_View
{

private:
	double		m_value;
	double		m_step;
};
#endif // 0

// =====================================================================
// SCUM_Table

class SCUM_Table : public SCUM_View
{
public:
	enum
	{
		kLines,
		kFilled,
		kWaveform
	};

	SCUM_Table(SCUM_Container* parent, PyrObject* obj);

	virtual void drawView();

	virtual bool mouseDown(int, const SCUM_Point&);
	virtual void mouseMove(int, const SCUM_Point&);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual SCUM_Size preferredSize();

protected:
	int indexFromPoint(const SCUM_Point& p);
	double valueFromPoint(const SCUM_Point& p);
	bool setValue(int x, double y, bool send);
	bool setValue(const SCUM_Point& p);

private:
	PyrObject*			m_data;
	int					m_size;
	int					m_style;
	double				m_step;
	bool				m_readOnly;
	SCUM_Point			m_prevPoint;
};

/*
  Slider (knob, filled)
  Table
  Range
  Knob
  Joystick
 */
#endif // SCUM_SLIDER_HH_INCLUDED
