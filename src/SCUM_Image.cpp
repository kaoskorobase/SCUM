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

	$Id: SCUM_Image.cpp,v 1.2 2004/08/04 11:48:26 steve Exp $
*/


#include "SCUM_Image.hh"

// SCUM_ImageHandle::~SCUM_ImageHandle()
// {
// }

SCUM::ImageHandle* SCUM_Image::handle()
{
	return 0;
}

SCUM_Size SCUM_Image::size() const
{
	return SCUM_Size();
}

// EOF
