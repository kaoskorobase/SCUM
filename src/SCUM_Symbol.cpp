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

	$Id: SCUM_Symbol.cpp,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#include "SCUM_Symbol.hh"
#include <PyrSymbol.h>

#define SCUM_DEF_SYMBOL(name)			PyrSymbol* SCUM_Symbol::name = 0;
#define SCUM_GET_SYMBOL(name, value)	SCUM_Symbol::name = getsym(#value)

// classes
SCUM_DEF_SYMBOL(Container);
SCUM_DEF_SYMBOL(Desktop);
SCUM_DEF_SYMBOL(Font);
SCUM_DEF_SYMBOL(Menu);
SCUM_DEF_SYMBOL(Size);
SCUM_DEF_SYMBOL(View);
SCUM_DEF_SYMBOL(Window);

// methods
SCUM_DEF_SYMBOL(doAction);
SCUM_DEF_SYMBOL(changed);
SCUM_DEF_SYMBOL(closeEvent);
SCUM_DEF_SYMBOL(contextMenu);
SCUM_DEF_SYMBOL(interpretCmdLine);
SCUM_DEF_SYMBOL(interpretPrintCmdLine);
SCUM_DEF_SYMBOL(keyDown);
SCUM_DEF_SYMBOL(keyUp);
SCUM_DEF_SYMBOL(mouseDown);
SCUM_DEF_SYMBOL(mouseMove);
SCUM_DEF_SYMBOL(mouseUp);
SCUM_DEF_SYMBOL(prDestroyed);
SCUM_DEF_SYMBOL(scrollWheel);
SCUM_DEF_SYMBOL(value);
SCUM_DEF_SYMBOL(x);
SCUM_DEF_SYMBOL(y);
SCUM_DEF_SYMBOL(willPopup);

void SCUM_Symbol::init()
{
	// classes
	SCUM_GET_SYMBOL(Container, SCUMContainer);
	SCUM_GET_SYMBOL(Desktop, SCUMDesktop);
	SCUM_GET_SYMBOL(Font, Font);
	SCUM_GET_SYMBOL(Menu, SCUMMenu);
	SCUM_GET_SYMBOL(Size, Size);
	SCUM_GET_SYMBOL(View, SCUMView);
	SCUM_GET_SYMBOL(Window, SCUMWindow);
	
	// methods
	SCUM_GET_SYMBOL(doAction, doAction);
	SCUM_GET_SYMBOL(changed, aspectChanged);
	SCUM_GET_SYMBOL(closeEvent, prHandleCloseEvent);
	SCUM_GET_SYMBOL(contextMenu, prHandleContextMenu);
	SCUM_GET_SYMBOL(interpretCmdLine, interpretCmdLine);
	SCUM_GET_SYMBOL(interpretPrintCmdLine, interpretPrintCmdLine);
	SCUM_GET_SYMBOL(keyDown, prHandleKeyDown);
	SCUM_GET_SYMBOL(keyUp, prHandleKeyUp);
	SCUM_GET_SYMBOL(mouseDown, prHandleMouseDown);
	SCUM_GET_SYMBOL(mouseMove, prHandleMouseMove);
	SCUM_GET_SYMBOL(mouseUp, prHandleMouseUp);
	SCUM_GET_SYMBOL(prDestroyed, prDestroyed);
	SCUM_GET_SYMBOL(scrollWheel, prHandleScrollWheel);
	SCUM_GET_SYMBOL(value, value);
	SCUM_GET_SYMBOL(x, x);
	SCUM_GET_SYMBOL(y, y);
	SCUM_GET_SYMBOL(willPopup, willPopup);
};

// EOF
