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

	$Id: SCUM_Slider.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_Slider.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"

#include <PyrKernel.h>
#include <PyrObject.h>

using namespace SCUM;

// =====================================================================
// SCUM_Slider

SCUM_Slider::SCUM_Slider(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_value(0.0),
	  m_step(0.0),
	  m_thumbSize(10),
	  m_steady(false)
{ }

void SCUM_Slider::drawView(const SCUM_Rect& damage)
{
	m_thumbRect = thumbRect();
	GCSave();
	GCSetColor(bgColor());
	GCFillRect(damage);
	GCDrawBeveledRect(bounds(), 1, true);
	GCSetColor(fgColor());
	GCFillRect(m_thumbRect);
	GCDrawBeveledRect(m_thumbRect, 1, false);
	GCRestore();
}

void SCUM_Slider::drawFocus(const SCUM_Rect& damage)
{
	SCUM_View::drawFocusInBounds(m_thumbRect.inset(1));
}

bool SCUM_Slider::mouseDown(int state, const SCUM_Point& where)
{
	if (m_steady) {
		m_steadyOffset = m_value - valueFromPoint(where);
	} else {
		setValue(valueFromPoint(where), true);
	}
	return true;
}

void SCUM_Slider::mouseMove(int state, const SCUM_Point& where)
{
	if (m_steady) {
		setValue(valueFromPoint(where) + m_steadyOffset, true);
	} else {
		setValue(valueFromPoint(where), true);
	}
}

void SCUM_Slider::scrollWheel(int state, const SCUM_Point&, const SCUM_Point& delta)
{
	double offset;

	if (m_step > 0.0) {
		offset = m_step;
	} else if (bounds().w > bounds().h) {
		offset = 1.0 / (bounds().w - m_thumbSize - 2);
	} else {
		offset = 1.0 / (bounds().h - m_thumbSize - 2);
	}
		
	if (state & kModMaskShift) offset *= 10;

	setValue(m_value - delta.y * offset, true);
}

void SCUM_Slider::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::value) {
		setBoolValue(slot, setValue(floatValue(slot), false));
	} else if (equal(key, "step")) {
		m_step = floatValue(slot);
		setBoolValue(slot, setValue(m_value, false));
	} else if (equal(key, "thumbSize")) {
		m_thumbSize = max(3, intValue(slot));
		updateLayout();
	} else if (equal(key, "steady")) {
		m_steady = boolValue(slot);
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_Slider::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::value) {
		setFloatValue(slot, m_value);
	} else if (equal(key, "step")) {
		setFloatValue(slot, m_step);
	} else if (equal(key, "thumbSize")) {
		setIntValue(slot, m_thumbSize);
	} else if (equal(key, "steady")) {
		setBoolValue(slot, m_steady);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

SCUM_Size SCUM_Slider::preferredSize()
{
	return SCUM_Size(m_thumbSize);
}

SCUM_Rect SCUM_Slider::thumbRect()
{
	bool horiz = (bounds().w > bounds().h);
	double range = (horiz ? bounds().w : bounds().h) - m_thumbSize - 2;
	SCUM_Rect r(bounds().inset(1));

	if (horiz) {
		r.x += m_value * range;
		r.w = m_thumbSize;
	} else {
		r.y += (1.0 - m_value) * range;
		r.h = m_thumbSize;
	}

	return r;
}

bool SCUM_Slider::setValue(double value, bool send)
{
	value = clip(value, 0.0, 1.0);
	
	if (m_step > 0.0) {
		value = floor(value / m_step + 0.5) * m_step;
	}

	if (value != m_value) {
		m_value = value;
		refresh();
		if (send) doAction();
		return true;
	}

	return false;
}

double SCUM_Slider::valueFromPoint(const SCUM_Point& p)
{
    double range, value;
    
    if (bounds().w > bounds().h) {
        range = bounds().w - m_thumbSize - 2;
        value = (p.x - bounds().x - 1 - m_thumbSize/2) / range;
    } else {
        range = bounds().h - m_thumbSize - 2;
        value = 1.0 - (p.y - bounds().y - 1 - m_thumbSize/2) / range;
    }

	return value;
}

// =====================================================================
// SCUM_Pad

SCUM_Pad::SCUM_Pad(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_x(.5), m_y(.5),
	  m_xStep(0.), m_yStep(0.),
	  m_steady(false)
{ }

void SCUM_Pad::drawView(const SCUM_Rect& damage)
{
	GCSave();
	GCSetColor(bgColor());
	GCFillRect(damage);
	GCDrawBeveledRect(bounds(), 1, true);
	GCSetColor(fgColor());
	GCSetLineWidth(1);
	SCUM_Rect r(bounds().inset(1));
	float x, y;
	x = min(r.x + (r.w * m_x), r.maxX() - 1.);
	y = min(r.y + (r.h * m_y), r.maxY() - 1.);
	GCDrawLine(x, r.y, x, r.maxY());
	GCDrawLine(r.x, y, r.maxX(), y);
	GCRestore();
}

bool SCUM_Pad::mouseDown(int state, const SCUM_Point& where)
{
	double x, y;

	valueFromPoint(where, x, y);

	if (m_steady) {
		m_steadyOffset.x = m_x - x;
		m_steadyOffset.y = m_y - y;
	} else {
		setValue(x, y, true);
	}
	return true;
}

void SCUM_Pad::mouseMove(int state, const SCUM_Point& where)
{
	double x, y;

	valueFromPoint(where, x, y);

	if (m_steady) {
		setValue(x + m_steadyOffset.x, y + m_steadyOffset.y, true);
	} else {
		setValue(x, y, true);
	}
}

void SCUM_Pad::scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
}

SCUM_Size SCUM_Pad::preferredSize()
{
	return SCUM_Size(10, 10);
}

void SCUM_Pad::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::x) {
		setBoolValue(slot, setValue(floatValue(slot), m_y, false));
	} else if (key == SCUM_Symbol::y) {
		setBoolValue(slot, setValue(m_x, floatValue(slot), false));
	} else if (equal(key, "xStep")) {
		m_xStep = floatValue(slot);
		setBoolValue(slot, setValue(m_x, m_y, false));
	} else if (equal(key, "yStep")) {
		m_yStep = floatValue(slot);
		setBoolValue(slot, setValue(m_x, m_y, false));
	} else if (equal(key, "steady")) {
		m_steady = boolValue(slot);
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_Pad::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::x) {
		setFloatValue(slot, m_x);
	} else if (key == SCUM_Symbol::y) {
		setFloatValue(slot, 1.-m_y);
	} else if (equal(key, "xStep")) {
		setFloatValue(slot, m_xStep);
	} else if (equal(key, "yStep")) {
		setFloatValue(slot, m_yStep);
	} else if (equal(key, "steady")) {
		setBoolValue(slot, m_steady);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

bool SCUM_Pad::setValue(double ix, double iy, bool send)
{
	bool changed = false;

	ix = clip(ix, 0., 1.);
	iy = clip(iy, 0., 1.);

	if (m_xStep > 0.) ix = floor(ix / m_xStep + 0.5) * m_xStep;
	if (m_yStep > 0.) iy = floor(iy / m_yStep + 0.5) * m_yStep;

	if (ix != m_x) {
		m_x = ix;
		changed = true;
	}

	if (iy != m_y) {
		m_y = iy;
		changed = true;
	}

	if (changed) {
		if (send) doAction();
		refresh();
		return true;
	}

	return false;
}

void SCUM_Pad::valueFromPoint(const SCUM_Point& p, double& ox, double& oy)
{
	ox = (p.x - bounds().x - 1.f) / (bounds().w - 2.f);
	oy = (p.y - bounds().y - 1.f) / (bounds().h - 2.f);
}

// =====================================================================
// SCUM_Table

SCUM_Table::SCUM_Table(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_data(0),
	  m_size(0),
	  m_style(kLines),
	  m_step(0.),
	  m_readOnly(false)
{ }

void SCUM_Table::drawView(const SCUM_Rect& damage)
{
	if (!m_data) return;

	GCSave();

	GCSetColor(bgColor());
	GCFillRect(damage);
	GCDrawBeveledRect(bounds(), 1, true);

	GCSetColor(fgColor());
	GCSetLineWidth(1);

	SCUM_Rect r(bounds().inset(1));
	int n = m_data->size;
	float* values = ((PyrFloatArray*)m_data)->f;

	if (n > 0) {
		int lasti = n - 1;
		int w = (int)floorf(r.w / n);
		float remain = r.w - w * n;
		float dec = remain > 0 ? n / remain : 0.f;
		float decacc = dec;
		float x = r.x;

		GCSetClip(r);

		for (int i=0; i < n; i++) {
			double y = r.maxY() - values[i] * r.h;
			int w1 = w;

			if (remain > 0) {
				if (i == lasti) {
					w1 += (int)floorf(remain);
				} else if (i > decacc) {
					w1 += 1;
					remain -= 1.f;
					decacc += dec;
				}
			}

			if (m_style == kLines) {
				GCDrawLine(x, y, x+w1, y);
			} else if (m_style == kFilled) {
				GCFillRect(x, y, w1, ceil(values[i] * r.h));
			} else if (m_style == kWaveform) {
				double value = values[i];
				double h2 = r.h * 0.5;
				if (value > 0.5) {
					GCFillRect(x, r.y + r.h * (1. - value), w1, r.h * (value - 0.5));
				} else {
					GCFillRect(x, r.y + r.h * 0.5, w1, r.h * (0.5 - value));
				}
			}

			x += w1;
		}
	}

	GCRestore();
}

bool SCUM_Table::mouseDown(int state, const SCUM_Point& where)
{
	if (!m_readOnly && bounds().inset(1).contains(where)) {
		if (state & kModMaskShift) {
			mouseMove(state, where);
			return false;
		} else {
			setValue(indexFromPoint(where), valueFromPoint(where), true);
			m_prevPoint = where;
			return true;
		}
	}
	return false;
}

void SCUM_Table::mouseMove(int state, const SCUM_Point& where)
{
	int prevIndex = indexFromPoint(m_prevPoint);
	int index = indexFromPoint(where);
	double prevValue = valueFromPoint(m_prevPoint);
	double value = valueFromPoint(where);

	if (prevIndex == index) {
		setValue(index, value, true);
	} else if (prevIndex < index) {
		double val = prevValue;
		double delta = (value - prevValue) / (index - prevIndex);
		for (int i=prevIndex; i <= index; ++i) {
			setValue(i, val, true);
			val += delta;
		}
	} else {
		double val = value;
		double delta = (prevValue - value) / (prevIndex - index);
		for (int i=index; i <= prevIndex; ++i) {
			setValue(i, val, true);
			val += delta;
		}
	}

	m_prevPoint = where;
}

SCUM_Size SCUM_Table::preferredSize()
{
	return SCUM_Size(m_size, 10).padded(1);
}

void SCUM_Table::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (key == SCUM_Symbol::value) {
		if (!isKindOfSlot(slot, class_floatarray)) throw TypeError();
		m_data = slot->uo;
		m_size = m_data->size;
		refresh();
	} else if (equal(key, "style")) {
		int style = m_style;
		if (getsym("lines") == slot->us) {
			style = kLines;
		} else if (getsym("filled") == slot->us) {
			style = kFilled;
		} else if (getsym("waveform") == slot->us) {
			style = kWaveform;
		}
		if (style != m_style) {
			m_style = style;
			refresh();
		}
	} else if (equal(key, "step")) {
		m_step = clip(floatValue(slot), 0., 1.);
		refresh();
	} else if (equal(key, "readOnly")) {
		m_readOnly = boolValue(slot);
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_Table::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "style")) {
		switch (m_style) {
			case kLines: SetSymbol(slot, getsym("lines")); break;
			case kFilled: SetSymbol(slot, getsym("filled")); break;
			case kWaveform: SetSymbol(slot, getsym("waveform")); break;
		}
	} else if (equal(key, "step")) {
		setFloatValue(slot, m_step);
	} else if (equal(key, "readOnly")) {
		setBoolValue(slot, m_readOnly);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

int SCUM_Table::indexFromPoint(const SCUM_Point& p)
{
	return (int)((p.x - bounds().x - 1.f) * m_size / bounds().w);
}

double SCUM_Table::valueFromPoint(const SCUM_Point& p)
{
	return 1.0 - (p.y - bounds().y - 1 - 1) / (bounds().h - 1 - 2);
}

bool SCUM_Table::setValue(int x, double y, bool send)
{
	bool changed;

	if (!m_size) return false;

	x = clip(x, 0, m_size - 1);
	y = clip(y, 0., 1.);

	if (m_step > 0.) {
        y = floor(y / m_step + 0.5) * m_step;
    }

	float* values = ((PyrFloatArray*)m_data)->f;

	changed = values[x] != y;

	if (changed) {
		values[x] = y;
		if (send) doAction();
		refresh();
	}

    return changed;
}

bool SCUM_Table::setValue(const SCUM_Point& p)
{
	return setValue(indexFromPoint(p), valueFromPoint(p), true);
}

// EOF
