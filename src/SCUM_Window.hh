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

	$Id: SCUM_Window.hh,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#ifndef SCUM_WINDOW_HH_INCLUDED
#define SCUM_WINDOW_HH_INCLUDED

#include "SCUM_Container.hh"
#include <list>
#include <vector>

class SCUM_Window;

namespace SCUM
{
	// =====================================================================
	// SCUM_WindowHandle -- system window interface

	class WindowHandle
	{
	public:
		struct Flags
		{
			unsigned	shown : 1;
			unsigned	visible : 1;
			unsigned	focused : 1;
			unsigned	decorated : 1;
			unsigned	modal : 1;
			unsigned	fullscreen : 1;
		};

		static WindowHandle* create(SCUM_Window* view);

		// return bounds in screen coordinates
		virtual SCUM_Rect bounds() = 0;

		// return state flags
		virtual Flags flags() = 0;
		virtual void setFlags(const Flags& flags) = 0;

		// set window title
		virtual void setTitle(const char* str) = 0;
		// set window position in screen coordinates
		virtual void setPos(const SCUM_Point& pos) = 0;
		// set window size
		virtual void setSize(const SCUM_Size& size) = 0;
		// set minimum resizability constraint
		virtual void setMinSize(const SCUM_Size& size) = 0;
		// set maximum resizability constraint
		virtual void setMaxSize(const SCUM_Size& size) = 0;
		
		// show/raise window
		virtual void show() = 0;
		// hide window
		virtual void hide() = 0;
		
		// refresh damage rect
		virtual void refresh(const SCUM_Rect& damage) = 0;
	};
};

// =====================================================================
// SCUM_Window -- top level view container

/* NOTE: object ownership, closing windows

   closing the window handle (either via a window manager close event
   or by calling the close() member function) _first_ deletes the
   view, _then_ deletes the handle. order is important, because
   methods purely virtual in SCUM_WindowHandle are called in view
   destructors; these cannot be called in the handle's destructor.
 */
class SCUM_Window : public SCUM_Bin
{
	struct DeferredCommands
	{
		unsigned		destroy : 1;
		unsigned		show : 1;
		unsigned		hide : 1;
		unsigned		resize : 1;
		unsigned		updateLayout : 1;
		unsigned		xFit : 1;
		unsigned		yFit : 1;
		unsigned		fullscreen : 1;
	};

	friend class SCUM_View;
	friend class SCUM_Container;

public:
	SCUM_Window(SCUM_Container* parent, PyrObject* obj);
	virtual ~SCUM_Window();

	SCUM::WindowHandle* handle() { return m_handle; }

	virtual void destroy(bool now);
	void raise();
	void lower();

	// drawing
	void draw();

	// system events
	virtual void closeEvent();
	virtual void showEvent();
	virtual void hideEvent();
	virtual void resizeEvent(const SCUM_Rect& bounds);
	virtual void focusEvent(bool hasFocus);
// 	void pasteEvent(const std::string& data);

#if 0
	virtual void evtClose();
	virtual void evtShow();
	virtual void evtHide();
	virtual void evtResize(const SCUM_Rect& bounds);
	virtual void evtFocus(bool hasFocus);
#endif // 0

	virtual void mouseMove(int state, const SCUM_Point& where);
	virtual void mouseUp(int state, const SCUM_Point& where);
	virtual void keyDown(int state, wint_t key);
	virtual void keyUp(int state, wint_t key);

	// mouse, focus
	bool hasMouse(const SCUM_View* view) const { return view == m_mouseMoveView; }
	bool hasFocus(const SCUM_View* view) const { return view == m_focusView; }
	void unsetFocus(bool send);
	void resetFocus(bool send);
	bool shouldDrawFocus() const { return flags().wHasFocus; }
	void tabNextFocus();
	void tabPrevFocus();

	// layout
	virtual void updateLayout();
	void updateLayout(bool xFit, bool yFit);
	virtual SCUM_Size preferredSize();

	// properties
	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

protected:
	// mouse events
	void setMouseView(SCUM_View* view);

	// focus
	SCUM_View* focusView() { return m_focusView; }
	const SCUM_View* focusView() const { return m_focusView; }
	void setFocusView(SCUM_View* view);

	// layout
	void doUpdateLayout(bool xFit, bool yFit);

private:
	void deferredAction(SCUM_Timer*);

private:
	SCUM::WindowHandle*			m_handle;
	SCUM_Timer*					m_deferredTimer;
	DeferredCommands			m_deferredCommands;
	SCUM_Size					m_initialSize;
	SCUM_Rect					m_screenBounds;
	SCUM_View*					m_mouseMoveView;
	SCUM_View*					m_belowMouseView;
	ChildList					m_mouseOverViews;
	SCUM_View*					m_focusView;
};

#endif // SCUM_WINDOW_HH_INCLUDED
