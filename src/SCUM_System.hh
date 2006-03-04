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


#ifndef SCUM_SYSTEM_HH_INCLUDED
#define SCUM_SYSTEM_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Timer.hh"

namespace SCUM
{
    enum ModMask
	{
	    kModMaskShift	= (1 << 0),
	    kModMaskControl	= (1 << 1),
	    kModMaskCommand	= (1 << 2),
	    kModMaskKeypad	= (1 << 3)
	};

    SCUM_Size screenSize();
    double time();

    // actions
    void addTimer(SCUM_Timer* timer);
};

#endif // SCUM_SYSTEM_HH_INCLUDED
