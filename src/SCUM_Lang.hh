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

	$Id: SCUM_Lang.hh,v 1.2 2004/08/15 14:42:24 steve Exp $
*/


#ifndef SCUM_LANG_HH_INCLUDED
#define SCUM_LANG_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Color.hh"
#include "SCUM_Font.hh"

#include <list>
#include <string>
#include <vector>
#include <SC_LanguageClient.h>

struct PyrClass;
struct PyrObject;
struct PyrSymbol;
typedef union pyrslot PyrSlot;

extern "C" PyrSymbol* getsym(const char*);

class SCUM_Object
{
public:
	SCUM_Object(PyrObject* pyrObj=0);
	virtual ~SCUM_Object();

	virtual void destroy(bool now);
	SC_LanguageClient* lang() { return SC_LanguageClient::instance(); }
	PyrObject* pyrObj() { return m_pyrObj; }
	void setPyrObj(PyrObject* obj);

	// property access
	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	static SCUM_Object* getObject(PyrObject* pyrObj);
	static SCUM_Object* getObject(PyrSlot* slot);

	template <class T> static T* getObject(PyrObject* obj)
	{
		return dynamic_cast<T*>(getObject(obj));
	}
	template <class T> static T* getObject(PyrSlot* slot)
	{
		return dynamic_cast<T*>(getObject(slot));
	}

protected:
	virtual void pyrObjChanged(PyrObject* oldObj);
	void sendMessage(PyrSymbol* method, size_t argc, PyrSlot* argv, PyrSlot* result=0);
	void sendMessage(const char* method, size_t argc, PyrSlot* argv, PyrSlot* result=0);
	void changed(PyrSymbol* what);
	void changed(const char* what);

private:
	PyrObject* m_pyrObj;
};

typedef std::list<SCUM_Object*> SCUM_ObjectList;
typedef SCUM_ObjectList::iterator SCUM_ObjectIter;

namespace SCUM
{
	struct Error
	{
		Error(int argCode)
			: code(argCode)
		{ }
		int code;
	};

	struct TypeError : public Error
	{
		TypeError();
	};

	inline void checkError(int err) { if (err) throw Error(err); }

	bool equal(const PyrSymbol* sym, const char* str);
	bool equal(const char* str, const PyrSymbol* sym);
	bool equal(const PyrSymbol* sym, const std::string& str);
	bool equal(const std::string& str, const PyrSymbol* sym);

	void checkKindOf(PyrSlot* slot, PyrClass* klass);

	double floatValue(PyrSlot* slot);
	void floatValues(PyrSlot* slot, std::vector<double>& array);
	int intValue(PyrSlot* slot);
	bool boolValue(PyrSlot* slot);
	char charValue(PyrSlot* slot);
	PyrSymbol* symbolValue(PyrSlot* slot);
	const char* stringValue(PyrSlot* slot, size_t& outSize);
	std::string stringValue(PyrSlot* slot);
	void stringValues(PyrSlot* slot, std::vector<std::string>& array);
	SCUM_Point pointValue(PyrSlot* slot);
	SCUM_Size sizeValue(PyrSlot* slot);
	SCUM_Rect rectValue(PyrSlot* slot);
	SCUM_Color colorValue(PyrSlot* slot);
	SCUM_Font fontValue(PyrSlot* slot);
	Border borderValue(PyrSlot* slot);

	void setNilValue(PyrSlot* slot);
	void setFloatValue(PyrSlot* slot, double v);
	void setIntValue(PyrSlot* slot, int v);
	void setBoolValue(PyrSlot* slot, bool v);
	void setPointValue(PyrSlot* slot, const SCUM_Point& v);
	void setSizeValue(PyrSlot* slot, const SCUM_Size& v);
	void setRectValue(PyrSlot* slot, const SCUM_Rect& v);
	void setColorValue(PyrSlot* slot, const SCUM_Color& v);
	void setBorderValue(PyrSlot* slot, Border v);

	class PyrIterator
	{
	public:
		PyrIterator(PyrSlot* slot);

		bool atEnd() const;
		size_t remain() const;
		PyrSlot* next();

	private:
		PyrSlot*		m_ptr;
		PyrSlot*		m_end;
	};
};

#endif // SCUM_LANG_HH_INCLUDED
