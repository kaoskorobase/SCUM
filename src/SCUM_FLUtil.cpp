/*  -*- mode: c++; indent-tabs-mode: t; c-basic-offset: 4 -*-
    vi: noet sta sw=4:

    SCUM. copyright (c) 2004, 2005 stefan kersten.

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
#include "SCUM_NSKey.h"

wint_t SCUM::FLEventKey(int& state)
{
    const int key = Fl::event_key();
		
    // printable ascii
    if ((key > 0) && (key < 256)) {
	int c = Fl::event_text()[0];
	if (isprint(c) && !iscntrl(c)) {
	    return c;
	} else {
	    return key;
	}
    }

    // keypad
    if ((key > FL_KP) && (key <= FL_KP_Last)) {
	state |= SCUM::kModMaskKeypad;
	return key - FL_KP;
    }

    // function keys
    if ((key > FL_F) && (key <= FL_F_Last)) {
	return NSF1FunctionKey + (key - FL_F) - 1;
    }
		
    switch (key) {
    case FL_Escape:
	return 0x1b;
    case FL_BackSpace:
	return 0x08;
    case FL_Tab:
	return 0x09;
    case 0xfe20: // backtab
	state |= SCUM::kModMaskShift;
	return 0x09;
    case FL_Enter:
	return 0x0a;
    case FL_Print:
	return NSPrintFunctionKey;
    case FL_Scroll_Lock:
	return NSScrollLockFunctionKey;
    case FL_Pause:
	return NSPauseFunctionKey;
    case FL_Insert:
	return NSInsertFunctionKey;
    case FL_Home:
	return NSHomeFunctionKey;
    case FL_Page_Up:
	return NSPageUpFunctionKey;
    case FL_Delete:
	return NSDeleteFunctionKey;
    case FL_End:
	return NSEndFunctionKey;
    case FL_Page_Down:
	return NSPageDownFunctionKey;
    case FL_Left:
	return NSLeftArrowFunctionKey;
    case FL_Up:
	return NSUpArrowFunctionKey;
    case FL_Right:
	return NSRightArrowFunctionKey;
    case FL_Down:
	return NSDownArrowFunctionKey;
    case FL_Shift_L:
    case FL_Shift_R:
	state |= SCUM::kModMaskShift;
	return 0;
    case FL_Control_L:
    case FL_Control_R:
	state |= SCUM::kModMaskControl;
	return 0;
    case FL_Caps_Lock:
	return 0;
    case FL_Alt_L:
    case FL_Alt_R:
    case FL_Meta_L:
    case FL_Meta_R:
	state |= SCUM::kModMaskCommand;
	return 0;
    case FL_Menu: return NSMenuFunctionKey;
    case FL_Num_Lock:
	return 0;
    case FL_KP_Enter:
	state |= SCUM::kModMaskKeypad;
	return 0x0a;
    }

    return 0;
}
