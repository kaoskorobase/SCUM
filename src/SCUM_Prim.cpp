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

	$Id: SCUM_Prim.cpp,v 1.2 2004/08/15 14:42:24 steve Exp $
*/


#include "SCUM_Prim.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Font.hh"
#include "SCUM_Menu.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"
#include "SCUM_View.hh"
#include "SCUM_ViewFactory.hh"
#include "SCUM_Window.hh"

#include <stdlib.h>

#include <PyrPrimitive.h>
#include <PyrKernel.h>
#include <VMGlobals.h>

namespace SCUM_Prim
{
	int Object_SetProperty(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;

		PyrSlot* args = g->sp - 2;
	
		if (!IsSym(args+1)) return errWrongType;
	
		SCUM_Object* obj = SCUM_Object::getObject(args[0].uo);
		if (!obj) return errFailed;

		try {
			obj->setProperty(args[1].us, args+2);
		} catch (SCUM::Error& e) {
			return e.code;
		}
        
        args[0].ucopy = args[2].ucopy;
        
		return errNone;
	}

	int Object_GetProperty(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;

		PyrSlot* args = g->sp - 2;
	
		if (!IsSym(args+1)) return errWrongType;
	
		SCUM_Object* obj = SCUM_Object::getObject(args[0].uo);
		if (!obj) return errFailed;

		try {
			obj->getProperty(args[1].us, args+2);
		} catch (SCUM::Error& e) {
			return e.code;
		}

        args[0].ucopy = args[2].ucopy;

		return errNone;
	}

	int Object_Destroy(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;

		PyrSlot* args = g->sp;

		SCUM_Object* obj = SCUM_Object::getObject(args[0].uo);
		if (!obj) return errFailed;

		obj->destroy(false);

		return errNone;
	}

	int Desktop_New(struct VMGlobals *g, int /* numArgsPushed */)
	{
		SCUM_Desktop::instance().setPyrObj(g->sp[0].uo);
	}

	int View_New(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;
		
		// view, parent, viewclass
		PyrSlot* args = g->sp - 2;

		SCUM_Container* parent = 0;

		if (isKindOfSlot(args+0, SCUM_Symbol::Window->u.classobj)) {
			if (!IsNil(args+1))
				return errWrongType;
			parent = 0;
		} else {
			if (!isKindOfSlot(args+1, SCUM_Symbol::Container->u.classobj))
				return errWrongType;
			parent = (SCUM_Container*)SCUM_Object::getObject(args[1].uo);
			if (!parent) return errFailed;
		}

		SCUM_View* view
			= SCUM_ViewFactory::instance().makeView(
				args[2].uoc->name.us->name, parent, args[0].uo
				);
		if (!view) return errFailed;

		return errNone;
	}

	int View_Raise(struct VMGlobals* g, int)
	{
		PyrSlot* arg = g->sp;
		SCUM_View* view = (SCUM_View*)SCUM_Object::getObject(arg);
		if (!view) return errFailed;
		view->raise();
		return errNone;
	}

	int View_Lower(struct VMGlobals* g, int)
	{
		PyrSlot* arg = g->sp;
		SCUM_View* view = (SCUM_View*)SCUM_Object::getObject(arg);
		if (!view) return errFailed;
		view->lower();
		return errNone;
	}

	int Window_UpdateLayout(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;

		// window, xFit, yFit
		PyrSlot* args = g->sp - 2;

		SCUM_Window* window = (SCUM_Window*)SCUM_Object::getObject(args[0].uo);
		if (!window) return errFailed;

		window->updateLayout(IsTrue(args+1), IsTrue(args+2));

		return errNone;
	}

	int Window_AdvanceFocus(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;

		// window, direction
		PyrSlot* args = g->sp - 1;

		SCUM_Window* window = (SCUM_Window*)SCUM_Object::getObject(args[0].uo);
		if (!window) return errFailed;

		int dir = SCUM::intValue(args+1);
		if (dir < 0) window->tabPrevFocus();
		else if (dir > 0) window->tabNextFocus();

		return errNone;
	}

	int Menu_New(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;
		
		// menu, items
		PyrSlot* args = g->sp - 1;
		
		if (!isKindOfSlot(args+1, class_array))
			return errWrongType;

		int itemSize = args[1].uo->size;
		PyrSlot* itemSlots = args[1].uo->slots;
		SCUM_MenuItems items;

		if (itemSize > 0) {
			try {
				while (itemSize--) items.push_back(itemSlots++);
			} catch (SCUM::Error& e) {
				return e.code;
			}
		}

		new SCUM_Menu(args[0].uo, items);

		return errNone;
	}

	int Menu_Popup(struct VMGlobals *g, int /* numArgsPushed */)
	{
		if (!g->canCallOS) return errCantCallOS;
		
		// menu, pos, item
		PyrSlot* args = g->sp - 2;

		SCUM_Menu* menu = (SCUM_Menu*)SCUM_Object::getObject(args+0);
		if (!menu) return errFailed;

		try {
			SCUM_Point pos = SCUM::pointValue(args+1);
			int item = SCUM::intValue(args+2);
			item = menu->popup(pos, item, false);
			SetInt(args+0, item);
		} catch (SCUM::Error& e) {
			return e.code;
		}

		return errNone;
	}

	int Font_Measure(struct VMGlobals* g, int)
	{
		// font, string, size
		PyrSlot* args = g->sp - 2;

		try {
			SCUM_Font font(SCUM::fontValue(args+0));
			std::string str(SCUM::stringValue(args+1));
			SCUM_TextExtents extents(font.measure(str.c_str()));
			SCUM::setSizeValue(args+2, extents.size);
		} catch (SCUM::Error& e) {
			return e.code;
		}
		
		args[0].ucopy = args[2].ucopy;

		return errNone;
	}

};

void SCUM_Prim::init()
{
	int base = nextPrimitiveIndex();
	int index = 0;

	definePrimitive(base, index++, "_SCUM_Object_SetProperty", SCUM_Prim::Object_SetProperty, 3, 0);
	definePrimitive(base, index++, "_SCUM_Object_GetProperty", SCUM_Prim::Object_GetProperty, 3, 0);	
	definePrimitive(base, index++, "_SCUM_Object_Destroy", SCUM_Prim::Object_Destroy, 1, 0);
	definePrimitive(base, index++, "_SCUM_Desktop_New", SCUM_Prim::Desktop_New, 1, 0);
	definePrimitive(base, index++, "_SCUM_View_New", SCUM_Prim::View_New, 3, 0);
	definePrimitive(base, index++, "_SCUM_View_Raise", SCUM_Prim::View_Raise, 1, 0);
	definePrimitive(base, index++, "_SCUM_View_Lower", SCUM_Prim::View_Lower, 1, 0);
// 	definePrimitive(base, index++, "_SCUM_View_FindByID", prSCView_FindByID, 2, 0);	
// 	definePrimitive(base, index++, "_SCUM_View_Focus", prSCView_Focus, 2, 0);	
 	definePrimitive(base, index++, "_SCUM_Window_UpdateLayout", SCUM_Prim::Window_UpdateLayout, 3, 0);
 	definePrimitive(base, index++, "_SCUM_Window_AdvanceFocus", SCUM_Prim::Window_AdvanceFocus, 2, 0);
	definePrimitive(base, index++, "_SCUM_Menu_New", SCUM_Prim::Menu_New, 2, 0);
	definePrimitive(base, index++, "_SCUM_Menu_Popup", SCUM_Prim::Menu_Popup, 3, 0);
	definePrimitive(base, index++, "_Font_Measure", SCUM_Prim::Font_Measure, 3, 0);
}

// EOF
