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


#ifndef SCUM_VIEWFACTORY_HH_INCLUDED
#define SCUM_VIEWFACTORY_HH_INCLUDED

#include "SCUM_View.hh"
#include <map>
#include <string>

class SCUM_ViewMakerBase
{
public:
	virtual SCUM_View* makeView(SCUM_Client* client, int oid, SCUM_Container* parent) = 0;
};

template <class T> class SCUM_ViewMaker : public SCUM_ViewMakerBase
{
public:
	SCUM_View* makeView(SCUM_Client* client, int oid, SCUM_Container* parent)
	{
		return new T(client, oid, parent);
	}
};

class SCUM_ViewFactory
{
public:
	SCUM_ViewFactory();

	static SCUM_ViewFactory& instance();

	void add(const char* className, SCUM_ViewMakerBase* maker);
	SCUM_View* makeView(const char* className, SCUM_Client* client, int oid, SCUM_Container* parent);

	static void init();

private:
	typedef std::map<std::string,SCUM_ViewMakerBase*> MakerMap;
	typedef MakerMap::iterator MakerIter;

	MakerMap m_makers;
};

#endif // SCUM_VIEWFACTORY_HH_INCLUDED
