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


#ifndef SCUM_TIMER_HH_INCLUDED
#define SCUM_TIMER_HH_INCLUDED

class SCUM_Timer
{
public:
    SCUM_Timer(float timeout)
	: m_valid(true), m_timeout(timeout)
    { }

    bool isValid() const { return m_valid; }
    void remove() { m_valid = false; }

    float timeout() const { return m_timeout; }
    void setTimeout(float timeout) { m_timeout = timeout; }

    virtual void operator () () = 0;

private:
    bool				m_valid;
    float				m_timeout;
};

template <class T> class SCUM_MemTimer : public SCUM_Timer
{
public:
    typedef void (T::*Callback)(SCUM_Timer* timer);

    SCUM_MemTimer(float timeout, Callback cb, T* obj)
	: SCUM_Timer(timeout),
	  m_cb(cb), m_obj(obj)
    { }

    virtual void operator () () { (m_obj->*m_cb)(this); }

private:
    Callback			m_cb;
    T*					m_obj;
};

#endif // SCUM_TIMER_HH_INCLUDED
