/* -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
   vi: et sta sw=4:

   SCUM. copyright (c) 2004 stefan kersten.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA

   $Id$
*/


#include "SCUM_Class.hh"
#include "SCUM_Client.hh"
#include "SCUM_Object.hh"

SCUM_ClassRegistry::SCUM_ClassRegistry()
{
    m_classes = st_init_strtable();
}

SCUM_ClassRegistry& SCUM_ClassRegistry::instance()
{
    static SCUM_ClassRegistry s_instance;
    return s_instance;
}

void SCUM_ClassRegistry::addClass(const char* name, SCUM_Class* theClass)
{
    st_insert(m_classes, (st_data_t)name, (st_data_t)theClass);
}

SCUM_Class* SCUM_ClassRegistry::lookupClass(const char* name)
{
    st_data_t value;
    if (st_lookup(m_classes, (st_data_t)name, &value))
        return (SCUM_Class*)value;
    return 0;
}

SCUM_Class::SCUM_Class(SCUM_ClassRegistry* reg, const char* name, const char* superclassName)
    : m_reg(reg),
      m_name(name),
      m_superclassName(superclassName),
      m_superclass(0)
{
    m_methods = st_init_strtable();
    m_reg->addClass(m_name, this);
}

SCUM_Class* SCUM_Class::getSuperclass()
{
    if (!m_superclassName)
        // Object
        return 0;
    if (!m_superclass) {
        m_superclass = m_reg->lookupClass(m_superclassName);
        if (!m_superclass) {
            throw std::runtime_error("class not found");
        }
    }
    return m_superclass;
}

SCUM_Object* SCUM_Class::makeObject(SCUM_Client* client, int oid)
{
    SCUM_ArgStream args;
    return makeObject(client, oid, args);
}

void SCUM_Class::addClosure(const char* name, void* closure)
{
    st_insert(m_methods, (st_data_t)name, (st_data_t)closure);
}

void* SCUM_Class::lookupMethod(const char* name)
{
    st_data_t value;
    if (st_lookup(m_methods, (st_data_t)name, &value))
        return (void*)value;
    return 0;
}
