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


#include "SCUM_Button.hh"
#include "SCUM_Container.hh"
#include "SCUM_Graph.hh"
#include "SCUM_Slider.hh"
#include "SCUM_Text.hh"
#include "SCUM_View.hh"
#include "SCUM_ViewFactory.hh"
#include "SCUM_Window.hh"

SCUM_ViewFactory::SCUM_ViewFactory()
{
	add("SCUMBang", new SCUM_ViewMaker<SCUM_Bang>());
	add("SCUMBin", new SCUM_ViewMaker<SCUM_Bin>());
	add("SCUMButton", new SCUM_ViewMaker<SCUM_Button>());
	//add("SCUMCanvasView", new SCUM_ViewMaker<SCUM_CanvasView>());
	add("SCUMChoice", new SCUM_ViewMaker<SCUM_Choice>());
	add("SCUMHBox", new SCUM_ViewMaker<SCUM_HBox>());
	add("SCUMHGrid", new SCUM_ViewMaker<SCUM_HGrid>());
	add("SCUMHSlider", new SCUM_ViewMaker<SCUM_HSlider>());
	add("SCUMLabel", new SCUM_ViewMaker<SCUM_Label>());
	add("SCUMList", new SCUM_ViewMaker<SCUM_List>());
	add("SCUMPad", new SCUM_ViewMaker<SCUM_Pad>());
	add("SCUMScope", new SCUM_ViewMaker<SCUM_Scope>());
	add("SCUMStringEntry", new SCUM_ViewMaker<SCUM_StringEntry>());
	add("SCUMTable", new SCUM_ViewMaker<SCUM_Table>());
	add("SCUMToggle", new SCUM_ViewMaker<SCUM_Toggle>());
	add("SCUMVBox", new SCUM_ViewMaker<SCUM_VBox>());
	add("SCUMVGrid", new SCUM_ViewMaker<SCUM_VGrid>());
	add("SCUMVSlider", new SCUM_ViewMaker<SCUM_VSlider>());
	add("SCUMView", new SCUM_ViewMaker<SCUM_View>());
	add("SCUMWindow", new SCUM_ViewMaker<SCUM_Window>());
}

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

SCUM_View* SCUM_ViewFactory::makeView(const char* className, SCUM_Client* client, int oid, SCUM_Container* parent)
{
	MakerIter iter = m_makers.find(className);
	if (iter == m_makers.end()) return 0;
	return iter->second->makeView(client, oid, parent);
}

// EOF
