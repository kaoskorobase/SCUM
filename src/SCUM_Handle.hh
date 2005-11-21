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


#ifndef SCUM_HANDLE_HH_INCLUDED
#define SCUM_HANDLE_HH_INCLUDED

#ifdef SCUM_DEBUG
# include <stdio.h>
# include <stdlib.h>
#endif

class SCUM_Handle
{
public:
    SCUM_Handle();

    inline void retain();
    inline void release();

protected:
    virtual ~SCUM_Handle() = 0;
    virtual void destroy();

private:
    int m_refcount;
};

inline void SCUM_Handle::retain()
{
    m_refcount++;
}

inline void SCUM_Handle::release()
{
#ifdef SCUM_DEBUG
    if (m_refcount <= 0) {
	fprintf(stderr, "SCUM_Handle::release: invalid reference count %d\n", m_refcount);
	abort();
    }
#endif // SCUM_DEBUG
    if (--m_refcount == 0) destroy();
}

#endif // SCUM_HANDLE_HH_INCLUDED
