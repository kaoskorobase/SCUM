/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vi: et sta sw=4:

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


#ifndef SCUM_MENU_HH_INCLUDED
#define SCUM_MENU_HH_INCLUDED

#include "SCUM_Handle.hh"
#include "SCUM_Object.hh"

#include <string>
#include <vector>

class SCUM_Menu;
class SCUM_View;

struct SCUM_MenuItem
{
    SCUM_MenuItem(int type, const std::string& text);
    SCUM_MenuItem(const SCUM_MenuItem& item);
    //SCUM_MenuItem(PyrSlot* spec);

    int type() const { return m_type; }
    const char* text() const { return m_text.c_str(); }

private:
    int					m_type;
    std::string			m_text;
};

typedef std::vector<SCUM_MenuItem> SCUM_MenuItems;

// class SCUM_MenuHandle
// {
// public:
// 	SCUM_MenuHandle();
// 	virtual ~SCUM_MenuHandle();

// 	virtual bool value(int item) const = 0;
// 	virtual void setValue(int item, bool value) = 0;
// 	virtual int popup(const SCUM_Point& where, int item) = 0;
// };

namespace SCUM
{
    enum
	{
            MenuAction		= '!',
            MenuSeparator	= '-',
            MenuToggle		= '?',
            MenuRadio		= '*',
            MenuOpen		= '>',
            MenuClose		= '<'
	};

    class MenuHandle : public SCUM_Handle
    {
    public:
        static MenuHandle* create(const std::vector<SCUM_MenuItem>& items);

        virtual bool value(int item) = 0;
        virtual void setValue(int item, bool value) = 0;
        virtual int popup(const SCUM_Point& where, int item) = 0;
    }; // MenuHandle
};

class SCUM_Menu : public SCUM_Object
{
public:
    SCUM_Menu(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
    virtual ~SCUM_Menu();

    SCUM::MenuHandle* handle() { return m_handle; }

    int item() const { return m_item; }
    void setItem(int item) { m_item = item; }

    bool value(int item) const;
    bool setValue(int item, bool value);

    int popup(const SCUM_Point& where, int item, bool send);

    virtual void setProperty(const char* key, SCUM_ArgStream& args);
    //virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

private:
    SCUM::MenuHandle*			m_handle;
    int							m_item;
};

#if 0
template <class T> class SCUM_Handle
{
    inline void retain()
    {
        if (m_cnt) (*m_cnt)++;
    }
    inline void release()
    {
        if (m_cnt && (--(*m_cnt) == 0)) {
            delete m_ptr;
            delete m_cnt;
        }
    }

public:
    SCUM_Handle()
        : m_ptr(0), m_cnt(0)
    { }
    SCUM_Handle(T* ptr)
        : m_ptr(ptr), m_cnt(new int(1))
    { }
    SCUM_Handle(const SCUM_Handle& handle)
        : m_ptr(handle.m_ptr), m_cnt(handle.m_cnt)
    {
        retain();
    }
    ~SCUM_Handle()
    {
        release();
    }

    bool operator == (const SCUM_Handle& handle)
    {
        return m_ptr == handle.m_ptr;
    }
    bool operator == (const T* ptr)
    {
        return m_ptr == ptr;
    }

    T& operator * () { return *m_ptr; }
	
    SCUM_Handle& operator = (const SCUM_Handle& handle)
    {
        if (m_ptr == handle.m_ptr) return *this;
        release();
        m_ptr = handle.m_ptr;
        m_cnt = handle.m_cnt;
        retain();
        return *this;
    }

private:
    T*			m_ptr;
    int*		m_cnt;
};
#endif // 0

#endif // SCUM_MENU_HH_INCLUDED
