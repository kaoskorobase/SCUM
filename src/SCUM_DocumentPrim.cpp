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

	$Id: SCUM_DocumentPrim.cpp,v 1.1 2004/08/15 14:42:23 steve Exp $
*/


#include "SCUM_Prim.hh"
#include "SCUM_Editor.hh"
#include "SCUM_FLApp.hh"
#include "SCUM_Lang.hh"

#include <PyrPrimitive.h>
#include <PyrKernel.h>
#include <VMGlobals.h>

namespace SCUM_Prim
{
	using namespace SCUM;

	int NumberOfOpenTextWindows(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;
		
		PyrSlot *a = g->sp;
		
		int num = SCUM_FLApp::instance()->documentManager()->size();
		SetInt(a, num);

		return errNone;
	}

	int TextWindow_GetIndexOfListener(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot* a = g->sp;

		int index = SCUM_FLApp::instance()->documentManager()->outputIndex();
		if (index < 0) return errFailed;

		SetInt(a, index);

        return errNone;
	}

	int TextWindow_GetByIndex(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp - 1;
		PyrSlot *b = g->sp; //the win number
        
        int err, inputIndex;
		err = slotIntVal(b, &inputIndex);
		if (err) return errWrongType;

        SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->at(inputIndex);
        if (!doc) return errFailed;

		//if there is a scobject already don't store i
		if (doc->pyrObj()){
			//post("docuemnt has scObject already\n");
			SetNil(a);
			return errNone;
		}

		doc->setPyrObj(a->uo);
		SetPtr(a->uo->slots + 0, doc);

		return errNone;
	}

	int TextWindow_GetLastIndex(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

        SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->last();
        if (!doc) return errFailed;

		if (doc->pyrObj()){
			//post("docuemnt has scObject already\n");
			SetNil(a);
			return errNone;
		}

		doc->setPyrObj(a->uo);
		SetPtr(a->uo->slots + 0, doc);

		return errNone;
	}

	int NewTextWindow(struct VMGlobals *g, int numArgsPushed)
	{
		SCUM_DocumentLangContext dlc;

        if (!g->canCallOS) return errCantCallOS;
		
        PyrSlot *d = g->sp - 3;
		PyrSlot *titleSlot = g->sp - 2;
        PyrSlot *stringSlot = g->sp - 1;
		PyrSlot *c = g->sp;
        
		if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

		SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->create();
        if (!doc) return errFailed;
        
		std::string str(stringValue(stringSlot));
		doc->insert(str.c_str());

		std::string title(stringValue(titleSlot));
		doc->setName(title.c_str());
		doc->makeWindow();
		doc->window()->show();

        if (IsTrue(c)) {
            SCUM_FLApp::instance()->documentManager()->setOutput(doc);
        }

		if (doc->pyrObj()){
			post("docuemnt has scObject already\n");
			SetNil(d);
			return errNone;
		}

		doc->setPyrObj(d->uo);
        SetPtr(d->uo->slots + 0, doc);

		return errNone;
	}

	int OpenTextFile(struct VMGlobals *g, int numArgsPushed)
	{
		SCUM_DocumentLangContext dlc;

        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *d = g->sp - 3;
        
		PyrSlot *a = g->sp - 2;
		PyrSlot *b = g->sp - 1;
		PyrSlot *c = g->sp;
		
		std::string path(stringValue(a));
		if (path.empty()) return errFailed;

        int rangeStart, rangeSize;
        int err = slotIntVal(b, &rangeStart);
        if (err) return err;
        err = slotIntVal(c, &rangeSize);
        if (err) return err;

		SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->open(path.c_str());
		if (!doc) {
			post("Can't open Document '%s'\n", path.c_str());
			return errFailed;
		}

        // select
		if (rangeStart > 0) {
			doc->insert_position(rangeStart);
			doc->show_insert_position();
		}
		if (rangeSize > 0) {
			doc->buffer()->select(rangeStart, rangeStart + rangeSize);
		}
		doc->makeWindow()->show();

		if (doc->pyrObj()) {
			SetNil(d);
			return errNone;
		}

		doc->setPyrObj(d->uo);
		SetPtr(d->uo->slots + 0, doc);

		return errNone;
	}

	int TextWindow_Close(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot* a = g->sp;
		
		SCUM_Document* doc = SCUM_Object::getObject<SCUM_Document>(a);
        if (!doc) return errFailed;

		SCUM_Document::Invocation* inv = doc->makeInvocation("close");
		inv->defer();
		
		return errNone;
	}

	int TextWindow_IsEdited(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;
		
		PyrSlot *a = g->sp;
		
		SCUM_Document* doc = SCUM_Object::getObject<SCUM_Document>(a);
        if (!doc) return errFailed;
		
		setBoolValue(a, doc->isEdited());

		return errNone;
	}

	int TextWindow_SetEditable(struct VMGlobals *g, int numArgsPushed)
	{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp-1;
		PyrSlot *b = g->sp;

		SCUM_Document* doc = SCUM_Object::getObject<SCUM_Document>(a);
        if (!doc) return errFailed;

		doc->setEditable(!IsFalse(b));

		return errNone;
	}

	int TextWindow_SetName(struct VMGlobals *g, int numArgsPushed)
	{
		if (!g->canCallOS) return errCantCallOS;

		PyrSlot* a = g->sp - 1;
		PyrSlot* b = g->sp;

		SCUM_Document* doc = SCUM_Object::getObject<SCUM_Document>(a);
        if (!doc) return errFailed;
		
		std::string title(stringValue(b));
		doc->setName(title.c_str());

		return errNone;
	}

};

void SCUM_Prim::initDocument()
{
	int base = nextPrimitiveIndex();
	int index = 0;

	definePrimitive(base, index++, "_NumberOfOpenTextWindows", NumberOfOpenTextWindows, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetIndexOfListener", TextWindow_GetIndexOfListener, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetByIndex", TextWindow_GetByIndex, 2, 0);
	definePrimitive(base, index++, "_TextWindow_GetLastIndex", TextWindow_GetLastIndex, 1, 0);
	definePrimitive(base, index++, "_NewTextWindow", NewTextWindow, 4, 0);
	definePrimitive(base, index++, "_OpenTextFile", OpenTextFile, 4, 0);
	definePrimitive(base, index++, "_TextWindow_Close", TextWindow_Close, 1, 0);
	definePrimitive(base, index++, "_TextWindow_IsEdited", TextWindow_IsEdited, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetEditable", TextWindow_SetEditable, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetName", TextWindow_SetName, 2, 0);	
}

// EOF
