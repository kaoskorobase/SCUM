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

	$Id: SCUM_Image.hh,v 1.3 2004/08/15 14:42:23 steve Exp $
*/


#ifndef SCUM_IMAGE_HH_INCLUDED
#define SCUM_IMAGE_HH_INCLUDED

#include "SCUM_Geometry.hh"
#include "SCUM_Handle.hh"

namespace SCUM
{
	class ImageHandle : public SCUM_Handle
	{
	public:
		static ImageHandle* create(const char* fileName, const SCUM_Size& size);

		virtual SCUM_Size size() = 0;
	};
};

class SCUM_Image
{
public:
	SCUM::ImageHandle* handle();
	SCUM_Size size() const;
};

#endif // SCUM_IMAGE_HH_INCLUDED
