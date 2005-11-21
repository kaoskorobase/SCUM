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


#ifndef SCUM_FLUTIL_HH_INCLUDED
#define SCUM_FLUTIL_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_System.hh"

#include <ctype.h>
#include <FL/Fl.H>

namespace SCUM
{
    inline Fl_Align convert(Align align);

    inline SCUM::ModMask FLEventState();
    inline SCUM_Point FLEventPos();
    wint_t FLEventKey(int& state);
};

inline Fl_Align SCUM::convert(Align align)
{
    int flalign;
    switch (align)
	{
	case kAlignNW: flalign = FL_ALIGN_TOP|FL_ALIGN_LEFT; break;
	case kAlignN: flalign = FL_ALIGN_TOP; break;
	case kAlignNE: flalign = FL_ALIGN_TOP|FL_ALIGN_RIGHT; break;
	case kAlignW: flalign = FL_ALIGN_CENTER|FL_ALIGN_LEFT; break;
	case kAlignC: flalign = FL_ALIGN_CENTER; break;
	case kAlignE: flalign = FL_ALIGN_CENTER|FL_ALIGN_RIGHT; break;
	case kAlignSW: flalign = FL_ALIGN_BOTTOM|FL_ALIGN_LEFT; break;
	case kAlignS: flalign = FL_ALIGN_BOTTOM; break;
	case kAlignSE: flalign = FL_ALIGN_BOTTOM|FL_ALIGN_RIGHT; break;
	};
    return (Fl_Align)flalign;
}

inline SCUM::ModMask SCUM::FLEventState()
{
    int flstate = Fl::event_state();
    int state = 0;
	
    if (flstate & FL_SHIFT)
	state |= SCUM::kModMaskShift;
    if (flstate & FL_CTRL)
	state |= SCUM::kModMaskControl;
    if ((flstate & FL_ALT) || (flstate & FL_META))
	state |= SCUM::kModMaskCommand;
	
    return (SCUM::ModMask)state;
}

inline SCUM_Point SCUM::FLEventPos()
{
    return SCUM_Point(Fl::event_x(), Fl::event_y());
}

#endif // SCUM_FLUTIL_HH_INCLUDED
