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

	$Id: SCUM_Image.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_IMAGE_HH_INCLUDED
#define SCUM_IMAGE_HH_INCLUDED

#include "SCUM_Geometry.hh"

class SCUM_ImageHandle
{
public:
	virtual ~SCUM_ImageHandle();
	virtual SCUM_Size size() = 0;
	virtual void draw(const SCUM_Point& p) = 0;
};

namespace SCUM
{
	SCUM_ImageHandle* makeImageHandle(const char* fileName, const SCUM_Size& size);
};

#endif // SCUM_IMAGE_HH_INCLUDED
