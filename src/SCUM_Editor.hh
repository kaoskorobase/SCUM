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

	$Id: SCUM_Editor.hh,v 1.1 2004/08/15 14:42:23 steve Exp $
*/


#ifndef SCUM_EDITOR_HH_INCLUDED
#define SCUM_EDITOR_HH_INCLUDED

#include "SCUM_Handle.hh"
#include "SCUM_Invocation.hh"
#include "SCUM_Lang.hh"
#include "SCUM_Menu.hh"

#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu.H>

#include <list>
#include <string>

class SCUM_Document;
class SCUM_DocumentView;
class SCUM_DocumentWindow;

// =====================================================================
// SCUM_Document

class SCUM_DocumentLangContext
{
public:
	SCUM_DocumentLangContext();
	~SCUM_DocumentLangContext();

	static bool state() { return s_state; }

private:
	static bool s_state;
};

class SCUM_Document : public SCUM_Object, public Fl_Text_Editor
{
	friend class SCUM_DocumentLangContext;
	friend class SCUM_DocumentManager;

public:
	class Invocation : public SCUM_Invocation
	{
	public:
		Invocation(SCUM_Document* doc, const char* t)
			: SCUM_Invocation(callback, doc),
			  type(t)
		{ }

		std::string		type;
		union
		{
		}				arg;

	private:
		static void callback(SCUM_Invocation* inv)
		{
			((SCUM_Document*)inv->obj())->execute((Invocation*)inv);
		}
	};

	inline bool inLangContext() const { return SCUM_DocumentLangContext::state(); }

	inline const std::string& name() const { return m_name; }
	void setName(const std::string& name);

	inline const std::string& path() const { return m_path; }
	inline bool hasFile() const { return !path().empty(); }

	bool isOutput() const;

	Fl_Window* makeWindow(int w=0, int h=0);

	inline bool isEditable() const { return m_editable; }
	void setEditable(bool flag);

	inline bool isEdited() const { return m_edited; }
	void setEdited(bool flag);

	void openFile(const char* path);
	bool saveToFile();
	bool canClose();
	void close();
	void showWindow();

	char* selectionOrLine();

	void sendMain(const char* method, const char* str="");
	void sendSelection(const char* method);

	void adjustTextSize(int amount);
	void executeSelection();

	Invocation* makeInvocation(const char* type);

protected:
	SCUM_Document(SCUM_DocumentManager* manager);
	virtual ~SCUM_Document();

	void setTitle();
	virtual int handle(int);
	void bufferModified(int pos, int ni, int nd, int nr, const char* del);

private:
	static void bufferModifyCB(int pos, int ni, int nd, int nr, const char* del, void* arg);
	void execute(Invocation* inv);
	
private:
	SCUM_DocumentManager*		m_manager;
	std::string					m_title;
	std::string					m_name;
	std::string					m_path;
	bool						m_editable;
	bool						m_edited;
	bool						m_richText;
};

// =====================================================================
// SCUM_DocumentManager

class SCUM_DocumentManager
{
	friend class SCUM_Document;
	friend class LangContext;

	typedef std::list<SCUM_Document*> List;

public:
	SCUM_DocumentManager();
	~SCUM_DocumentManager();

	size_t size() const { return m_list.size(); }

	int index(SCUM_Document* doc);
	SCUM_Document* at(int index);
	SCUM_Document* last();

	inline SCUM_Document* output() { return m_output; }
	void setOutput(SCUM_Document* doc);
	inline int outputIndex() { return index(output()); }

	SCUM_Document* create();
	SCUM_Document* open(const char* path);
	SCUM_Document* open();

	SCUM_Document* menuSelect(int x, int y, const char* title=0);
	SCUM_Document* shortcutSelect();

	void updateMenu();

private:
	void add(SCUM_Document* doc);
	void remove(SCUM_Document* doc);

	void rebuildMenu();

private:
	List				m_list;
	SCUM_Document*		m_output;
	Fl_Menu_Item*		m_menu;
};

// =====================================================================
// SCUM_DocumentWindow

class SCUM_DocumentWindow : public Fl_Window
{
public:
	SCUM_DocumentWindow(SCUM_Document* view, int w, int h);
	virtual ~SCUM_DocumentWindow();
};

#endif // SCUM_EDITOR_HH_INCLUDED
