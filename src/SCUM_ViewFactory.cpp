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

	$Id: SCUM_ViewFactory.cpp,v 1.2 2004/08/15 14:42:24 steve Exp $
*/


#include "SCUM_ViewFactory.hh"

SCUM_ViewFactory& SCUM_ViewFactory::instance()
{
	static SCUM_ViewFactory s_instance;
	return s_instance;
}

void SCUM_ViewFactory::add(const char* className, SCUM_ViewMakerBase* maker)
{
	MakerIter iter = m_makers.find(className);
	if (iter != m_makers.end())	delete iter->second;
	m_makers[className] = maker;
}

SCUM_View* SCUM_ViewFactory::makeView(const char* className, SCUM_Container* parent, PyrObject* obj)
{
	MakerIter iter = m_makers.find(className);
	if (iter == m_makers.end()) return 0;
	return iter->second->makeView(parent, obj);
}

#include "SCUM_Button.hh"
#include "SCUM_Canvas.hh"
#include "SCUM_Container.hh"
#include "SCUM_Graph.hh"
#include "SCUM_Text.hh"
#include "SCUM_View.hh"
#include "SCUM_Slider.hh"
#include "SCUM_Window.hh"

void SCUM_ViewFactory::init()
{
	SCUM_ViewFactory& f = SCUM_ViewFactory::instance();

	f.add("SCUMBang", new SCUM_ViewMaker<SCUM_Bang>());
	f.add("SCUMBin", new SCUM_ViewMaker<SCUM_Bin>());
	f.add("SCUMButton", new SCUM_ViewMaker<SCUM_Button>());
	f.add("SCUMCanvasView", new SCUM_ViewMaker<SCUM_CanvasView>());
	f.add("SCUMChoice", new SCUM_ViewMaker<SCUM_Choice>());
	f.add("SCUMHBox", new SCUM_ViewMaker<SCUM_HBox>());
	f.add("SCUMHGrid", new SCUM_ViewMaker<SCUM_HGrid>());
	f.add("SCUMLabel", new SCUM_ViewMaker<SCUM_Label>());
	f.add("SCUMList", new SCUM_ViewMaker<SCUM_List>());
	f.add("SCUMPad", new SCUM_ViewMaker<SCUM_Pad>());
	f.add("SCUMScope", new SCUM_ViewMaker<SCUM_Scope>());
	f.add("SCUMSlider", new SCUM_ViewMaker<SCUM_Slider>());
	f.add("SCUMStringEntry", new SCUM_ViewMaker<SCUM_StringEntry>());
	f.add("SCUMTable", new SCUM_ViewMaker<SCUM_Table>());
	f.add("SCUMToggle", new SCUM_ViewMaker<SCUM_Toggle>());
	f.add("SCUMVBox", new SCUM_ViewMaker<SCUM_VBox>());
	f.add("SCUMVGrid", new SCUM_ViewMaker<SCUM_VGrid>());
	f.add("SCUMView", new SCUM_ViewMaker<SCUM_View>());
	f.add("SCUMWindow", new SCUM_ViewMaker<SCUM_Window>());
}

// EOF
