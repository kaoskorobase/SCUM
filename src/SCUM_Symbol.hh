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

	$Id: SCUM_Symbol.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_SYMBOL_HH_INCLUDED
#define SCUM_SYMBOL_HH_INCLUDED

struct PyrSymbol;

#define SCUM_DECL_SYMBOL(name) extern PyrSymbol* name

namespace SCUM_Symbol
{
	// classes
	SCUM_DECL_SYMBOL(Container);
	SCUM_DECL_SYMBOL(Desktop);
	SCUM_DECL_SYMBOL(Font);
	SCUM_DECL_SYMBOL(Menu);
	SCUM_DECL_SYMBOL(Size);
	SCUM_DECL_SYMBOL(View);
	SCUM_DECL_SYMBOL(Window);

	// methods
	SCUM_DECL_SYMBOL(doAction);
	SCUM_DECL_SYMBOL(changed);
	SCUM_DECL_SYMBOL(closeEvent);
	SCUM_DECL_SYMBOL(contextMenu);
	SCUM_DECL_SYMBOL(interpretCmdLine);
	SCUM_DECL_SYMBOL(interpretPrintCmdLine);
	SCUM_DECL_SYMBOL(keyDown);
	SCUM_DECL_SYMBOL(keyUp);
	SCUM_DECL_SYMBOL(mouseDown);
	SCUM_DECL_SYMBOL(mouseMove);
	SCUM_DECL_SYMBOL(mouseUp);
	SCUM_DECL_SYMBOL(prDestroyed);
	SCUM_DECL_SYMBOL(scrollWheel);
	SCUM_DECL_SYMBOL(value);
	SCUM_DECL_SYMBOL(x);
	SCUM_DECL_SYMBOL(y);
	SCUM_DECL_SYMBOL(willPopup);

	void init();
};

#undef SCUM_DECL_SYMBOL

#endif // SCUM_SYMBOL_HH_INCLUDED
