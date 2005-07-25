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

	$Id$
*/


#ifndef SCUM_CLASS_HH_INCLUDED
#define SCUM_CLASS_HH_INCLUDED

#include "SCUM_Object.hh"
#include "st.h"

#include <stdexcept>

class SCUM_Class;
class SCUM_Client;

class SCUM_ClassRegistry
{
public:
	SCUM_ClassRegistry();

	static SCUM_ClassRegistry& instance();
	
	void addClass(const char* name, SCUM_Class* theClass);
	SCUM_Class* lookupClass(const char* name);

private:
	st_table*	m_classes;
};

class SCUM_Class
{
public:
	SCUM_Class(SCUM_ClassRegistry* reg, const char* name, const char* superclassName);

	SCUM_ClassRegistry* getRegistry() { return m_reg; }
	const char* getName() const { return m_name; }
	SCUM_Class* getSuperclass();

	virtual SCUM_Object* makeObject(SCUM_Client* client, int oid, SCUM_ArgStream& args) = 0;
	SCUM_Object* makeObject(SCUM_Client* client, int oid);
	virtual void dispatchMethod(SCUM_Object* obj, const char* method, SCUM_ArgStream& args) = 0;

protected:
	void addClosure(const char* name, void* closure);
	void* lookupMethod(const char* name);

private:
	SCUM_ClassRegistry*	m_reg;
	const char*			m_name;
	const char*			m_superclassName;
	SCUM_Class*			m_superclass;
	st_table*			m_methods;
};

template <class T> class SCUM_ClassT : public SCUM_Class
{
public:
	typedef void (T::*MethodFunc)(const char* name, SCUM_ArgStream& args);
	struct Method
	{
		MethodFunc	m_func;
	};

	SCUM_ClassT(SCUM_ClassRegistry* reg, const char* name, const char* superclassName)
		: SCUM_Class(reg, name, superclassName)
	{ }

	SCUM_Object* makeObject(SCUM_Client* client, int oid, SCUM_ArgStream& args)
	{
		return new T((SCUM_Class*)this, client, oid, args);
	}

	void dispatchMethod(SCUM_Object* obj, const char* name, SCUM_ArgStream& args)
	{
		T* tObj = dynamic_cast<T*>(obj);
		if (!tObj)
			throw std::runtime_error("wrong type");
		Method* method = (Method*)lookupMethod(name);
		if (method) {
			MethodFunc func = method->m_func;
			(tObj->*func)(name, args);
		} else {
			SCUM_Class* superclass = getSuperclass();
			if (!superclass)
				throw std::runtime_error("DNU");
			superclass->dispatchMethod(obj, name, args);
		}
	}

	void addMethod(const char* name, MethodFunc func)
	{
		Method* method = new Method;
		method->m_func = func;
		addClosure(name, (void*)method);
	}
};

#endif // SCUM_CLASS_HH_INCLUDED
