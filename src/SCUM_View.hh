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

	$Id: SCUM_View.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_VIEW_HH_INCLUDED
#define SCUM_VIEW_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Lang.hh"

#include <stdint.h>

class SCUM_Container;
class SCUM_Desktop;
class SCUM_Menu;
class SCUM_Timer;
class SCUM_Window;

struct SCUM_Layout
{
    SCUM_Layout()
		: alignment(SCUM::AlignC)
	{ }

	SCUM_Size			lastPreferredSize;		// cache for preferred view size
	SCUM_Size			minSize;				// minimum view size
	SCUM_Point			expand;					// factor for expanding into packing space
	SCUM_Point			fill;					// ratio for filling available space
	uint8_t				alignment;				// alignment for placement in available space
};

/*
  View coordinates are in window coordinates.
 */
class SCUM_View : public SCUM_Object
{
public:
	struct Flags
	{
		// View
		unsigned		vVisible : 1;
		unsigned		vEnabled : 1;
		unsigned		vCanFocus : 1;
		// Container
		unsigned		cHomogenous : 1;
		unsigned		cHasVisible : 1;
		// Window
		unsigned		wWasShown : 1;
		unsigned		wResizable : 1;
		unsigned		wHasFocus : 1;
	};

	friend class SCUM_Container;
	friend class SCUM_Window;

public:
	SCUM_View(SCUM_Container* parent, PyrObject* pyrObj);
	virtual ~SCUM_View();

	// hierarchy access
	SCUM_Container* parent() { return m_parent; }
	const SCUM_Container* parent() const { return m_parent; }
	SCUM_Window* window() { return m_window; }
	const SCUM_Window* window() const { return m_window; }
	SCUM_Desktop* desktop();

	virtual void raise();
	virtual void lower();

	bool isVisible() const { return m_flags.vVisible; }
	bool isEnabled() const { return m_flags.vEnabled; }

	// drawing
	void refresh(const SCUM_Rect& damage);
	virtual void refresh();

	virtual void draw();
	virtual void drawView();
	virtual void drawDisabled();
	virtual void drawFocus();

	// GL
	virtual void initGL();
	virtual void drawGL();
	virtual void resizeGL(int w, int h);

	// events
	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void mouseMove(int state, const SCUM_Point& where);
	virtual void mouseUp(int state, const SCUM_Point& where);
	virtual void scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta);
	virtual void contextMenu(int state, const SCUM_Point& where);
	virtual void keyDown(int state, wint_t key);
	virtual void keyUp(int state, wint_t key);

	// bounds
	const SCUM_Rect& bounds() const { return m_bounds; }
	
	SCUM_Size size() const { return bounds().size(); }
	bool contains(const SCUM_Point& point) const { return bounds().contains(point); }

	// layout
	bool layoutNeedsUpdate(const SCUM_Size& newSize) const;
	SCUM_Layout& layout() { return m_layout; }
	const SCUM_Size& updatePreferredSize();
	virtual SCUM_Size preferredSize();
	virtual void updateLayout();
	void setBounds(const SCUM_Rect& bounds);

	// mouse, focus
	bool hasMouse() const;
	bool hasFocus() const;
	virtual bool canFocus() const;
	virtual void makeFocus(bool focus, bool send);
	virtual SCUM_View* nextFocus(bool canFocus, bool& foundFocus);
	virtual SCUM_View* prevFocus(bool canFocus, bool& foundFocus);

	// property access
	void setProperty(const PyrSymbol* key, PyrSlot* slot);
	void getProperty(const PyrSymbol* key, PyrSlot* slot);

	const SCUM_Color& fgColor() const { return m_fgColor; }
	const SCUM_Color& bgColor() const { return m_bgColor; }

	// action
	void doAction(PyrSymbol* message);
	void doAction();

	// animation
	void startAnimation();
	void stopAnimation();
	virtual void animate();
	
protected:
	const Flags& flags() const { return m_flags; }
	Flags& flags() { return m_flags; }

	void drawFocus(const SCUM_Rect& bounds);

	bool sendMouseDown(int state, const SCUM_Point& where);
	void sendMouseMove(int state, const SCUM_Point& where);
	void sendMouseUp(int state, const SCUM_Point& where);
	void sendScrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta);

	virtual void boundsChanged(const SCUM_Rect& bounds);

private:
	void animateAction(SCUM_Timer* action);

protected:
	SCUM_Container*		m_parent;
	SCUM_Window*		m_window;
	Flags				m_flags;
	SCUM_Rect			m_bounds;
	SCUM_Layout			m_layout;
	SCUM_Color			m_fgColor;
	SCUM_Color			m_bgColor;
	SCUM_Timer*			m_animateTimer;
};

#endif // SCUM_VIEW_HH_INCLUDED
