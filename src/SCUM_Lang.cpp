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

	$Id: SCUM_Lang.cpp,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#include "SCUM_Lang.hh"
#include "SCUM_Font.hh"
#include "SCUM_Symbol.hh"

#include <string.h>

#include <PyrKernel.h>
#include <PyrObject.h>
#include <PyrSlot.h>
#include <PyrSymbol.h>
#include <VMGlobals.h>

// =====================================================================
// SCUM_Object

SCUM_Object::SCUM_Object(PyrObject* pyrObj)
	: m_pyrObj(0)
{
	setPyrObj(pyrObj);
}

SCUM_Object::~SCUM_Object()
{
	setPyrObj(0);
}

void SCUM_Object::destroy(bool /* now */)
{
	delete this;
}

void SCUM_Object::setPyrObj(PyrObject* obj)
{
	if (obj != m_pyrObj) {
		PyrObject* oldObj = m_pyrObj;
		m_pyrObj = obj;
		if (oldObj) SetNil(oldObj->slots+0);
		if (obj) SetPtr(obj->slots+0, this);
		pyrObjChanged(oldObj);
	}
}

void SCUM_Object::sendMessage(PyrSymbol* method, size_t argc, PyrSlot* argv, PyrSlot* result)
{
    if (pyrObj()) {
		lang()->lock();
        VMGlobals* g = lang()->getVMGlobals();
        g->canCallOS = true;
        ++g->sp; SetObject(g->sp, pyrObj());
        for (size_t i=0; i < argc; ++i) {
            ++g->sp; g->sp->ucopy = argv[i].ucopy; 
        }
        runInterpreter(g, method, argc+1);
        g->canCallOS = false;
        if (result) result->ucopy = g->result.ucopy;
		lang()->unlock();
    }
}

// void SCUM_Object::changed(PyrSymbol* what, PyrSlot* result)
// {
// 	PyrSlot args[1];
// 	SetSymbol(args+0, what);
// 	sendMessage(SCUM_Symbol::changed, 1, args, result);
// }

// void SCUM_Object::changed(const char* what, PyrSlot* result)
// {
// 	changed(getsym(what), result);
// }

void SCUM_Object::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
}

void SCUM_Object::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	SetNil(slot);
}

SCUM_Object* SCUM_Object::getObject(PyrObject* pyrObj)
{
	return (SCUM_Object*)pyrObj->slots[0].ui;
}

SCUM_Object* SCUM_Object::getObject(PyrSlot* slot)
{
	return (SCUM_Object*)slot->uo->slots[0].ui;
}

void SCUM_Object::pyrObjChanged(PyrObject* /* oldObj */)
{
}

// =====================================================================
// exceptions

SCUM::TypeError::TypeError()
	: SCUM::Error(errWrongType)
{ }

// =====================================================================
// symbol equality

bool SCUM::equal(const PyrSymbol* sym, const char* str)
{
	return strcmp(sym->name, str) == 0;
}

bool SCUM::equal(const char* str, const PyrSymbol* sym)
{
	return strcmp(sym->name, str) == 0;
}

bool SCUM::equal(const PyrSymbol* sym, const std::string& str)
{
	return strcmp(sym->name, str.c_str()) == 0;
}

bool SCUM::equal(const std::string& str, const PyrSymbol* sym)
{
	return strcmp(sym->name, str.c_str()) == 0;
}

// =====================================================================
// slot manipulation

inline static void SCUM_floatValues(float* dst, PyrSlot* src, size_t n)
{
	while (n--) SCUM::checkError(slotFloatVal(src++, dst++));
}

inline static void SCUM_setFloatValues(PyrSlot* dst, const float* src, size_t n)
{
	while (n--) SetFloat(dst++, *src++);
}

void SCUM::checkKindOf(PyrSlot* slot, PyrClass* klass)
{
	if (!isKindOfSlot(slot, klass)) throw TypeError();
}

double SCUM::floatValue(PyrSlot* slot)
{
	double v;
	checkError(slotDoubleVal(slot, &v));
	return v;
}

void SCUM::floatValues(PyrSlot* slot, std::vector<double>& array)
{
    if (!isKindOfSlot(slot, class_array)) throw TypeError();

	array.clear();

	int size = slot->uo->size;
	PyrSlot* slots = slot->uo->slots;

	if (size > 0) {
		array.reserve(size);
		while (size--) array.push_back(floatValue(slots++));
	}
}

int SCUM::intValue(PyrSlot* slot)
{
	int v;
	checkError(slotIntVal(slot, &v));
	return v;
}

bool SCUM::boolValue(PyrSlot* slot)
{
	return IsTrue(slot);
}

char SCUM::charValue(PyrSlot* slot)
{
	if (slot->utag == tagChar) {
		return slot->uc;
	} else if (IsInt(slot)) {
		return slot->ui & 0xFF;
	} else if (IsFloat(slot)) {
		return ((int)slot->uf) & 0xFF;
	}  
	throw TypeError();
}

std::string SCUM::stringValue(PyrSlot* slot)
{
	if (IsSym(slot)) {
		return std::string(slot->us->name);
	}
	if (isKindOfSlot(slot, class_string)) {
		return std::string(slot->uos->s, slot->uo->size);
	}
	throw TypeError();
}

void SCUM::stringValues(PyrSlot* slot, std::vector<std::string>& array)
{
    if (!isKindOfSlot(slot, class_array)) throw TypeError();

	array.clear();

	int size = slot->uo->size;
	PyrSlot* slots = slot->uo->slots;

	if (size > 0) {
		array.reserve(size);
		while (size--) array.push_back(stringValue(slots++));
	}
}

SCUM_Point SCUM::pointValue(PyrSlot* slot)
{
	if (isKindOfSlot(slot, s_point->u.classobj)) {
		float v[2];
		SCUM_floatValues(v, slot->uo->slots, 2);
		return SCUM_Point(v[0], v[1]);
	}
	throw TypeError();
}

SCUM_Size SCUM::sizeValue(PyrSlot* slot)
{
	if (isKindOfSlot(slot, SCUM_Symbol::Size->u.classobj)) {
		float v[2];
		SCUM_floatValues(v, slot->uo->slots, 2);
		return SCUM_Size(v[0], v[1]);
	}
	throw TypeError();
}

SCUM_Rect SCUM::rectValue(PyrSlot* slot)
{
	if (isKindOfSlot(slot, s_rect->u.classobj)) {
		float v[4];
		SCUM_floatValues(v, slot->uo->slots, 4);
		return SCUM_Rect(v[0], v[1], v[2], v[3]);
	}
	throw TypeError();
}

SCUM_Color SCUM::colorValue(PyrSlot* slot)
{
	if (isKindOfSlot(slot, s_color->u.classobj)) {
		float v[4];
		SCUM_floatValues(v, slot->uo->slots, 4);
		return SCUM_Color(v[0], v[1], v[2], v[3]);
	}
	throw TypeError();
}

SCUM_Font SCUM::fontValue(PyrSlot* slot)
{
	if (isKindOfSlot(slot, SCUM_Symbol::Font->u.classobj)) {
		PyrSlot* slots = slot->uo->slots;
		std::string name(stringValue(slots+0));
		float size = floatValue(slots+1);
		return SCUM_Font(name.c_str(), SCUM::max(1, (int)size));
	}
	throw TypeError();
}

void SCUM::setNilValue(PyrSlot* slot)
{
	SetNil(slot);
}

void SCUM::setFloatValue(PyrSlot* slot, double v)
{
	SetFloat(slot, v);
}

void SCUM::setIntValue(PyrSlot* slot, int v)
{
	SetInt(slot, v);
}

void SCUM::setBoolValue(PyrSlot* slot, bool v)
{
	SetBool(slot, v);
}

void SCUM::setPointValue(PyrSlot* slot, const SCUM_Point& v)
{
	if (isKindOfSlot(slot, s_point->u.classobj)) {
		float fv[2];
		v.copy(fv);
		SCUM_setFloatValues(slot->uo->slots, fv, 2);
	} else {
		throw TypeError();
	}
}

void SCUM::setSizeValue(PyrSlot* slot, const SCUM_Size& v)
{
	if (isKindOfSlot(slot, SCUM_Symbol::Size->u.classobj)) {
		float fv[2];
		v.copy(fv);
		SCUM_setFloatValues(slot->uo->slots, fv, 2);
	} else {
		throw TypeError();
	}
}

void SCUM::setRectValue(PyrSlot* slot, const SCUM_Rect& v)
{
	if (isKindOfSlot(slot, s_rect->u.classobj)) {
		float fv[4];
		v.copy(fv);
		SCUM_setFloatValues(slot->uo->slots, fv, 4);
	} else {
		throw TypeError();
	}
}

void SCUM::setColorValue(PyrSlot* slot, const SCUM_Color& v)
{
	if (isKindOfSlot(slot, s_color->u.classobj)) {
		float fv[4];
		v.copy(fv);
		SCUM_setFloatValues(slot->uo->slots, fv, 4);
	} else {
		throw TypeError();
	}
}

// EOF
