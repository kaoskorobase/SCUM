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

	$Id: SCUM_Editor.cpp,v 1.1 2004/08/15 14:42:23 steve Exp $
*/


#include "SCUM_Editor.hh"
#include "SCUM_FLApp.hh"
#include "SCUM_Util.hh"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>

// =====================================================================
// SCUM_DocumentLangContext

bool SCUM_DocumentLangContext::s_state = false;

SCUM_DocumentLangContext::SCUM_DocumentLangContext()
{
	SCUM_ASSERT(!s_state);
	s_state = true;
}

SCUM_DocumentLangContext::~SCUM_DocumentLangContext()
{
	SCUM_ASSERT(s_state);
	s_state = false;
}

// =====================================================================
// SCUM_Document

#define SCUM_DOCUMENT(ptr) ((SCUM_Document*)(ptr))

extern int rtf2txt(char*);

namespace SCUM
{
	static void Document_closeCB(Fl_Widget* window, void* arg)
	{
		if (SCUM_DOCUMENT(arg)->canClose()) delete window;
	}

	static void Document_showCB(void* arg)
	{
		SCUM_DOCUMENT(arg)->showWindow();
	}

	static int kf_increaseFontSize(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->adjustTextSize(1);
		return 1;
	}
	static int kf_decreaseFontSize(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->adjustTextSize(-1);
		return 1;
	}
	static int kf_new(int c, Fl_Text_Editor* e)
	{
		SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->create();
		if (doc) doc->makeWindow()->show();
		return 1;
	}
	static int kf_open(int c, Fl_Text_Editor* e)
	{
		SCUM_Document* doc = SCUM_FLApp::instance()->documentManager()->open();
		if (doc) doc->makeWindow()->show();
		return 1;
	}
	static int kf_save(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->saveToFile();
		return 1;
	}
	static int kf_close(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->close();
		return 1;
	}
	static int kf_quit(int c, Fl_Text_Editor* e)
	{
		SCUM_FLApp::instance()->quit(0);
		return 1;
	}
	static int kf_executeSelection(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->executeSelection();
		return 1;
	}
	static int kf_methodTemplates(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->sendSelection("methodTemplates");
		return 1;		
	}
	static int kf_methodReferences(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->sendSelection("methodReferences");
		return 1;
	}
	static int kf_openCodeFile(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->sendSelection("openCodeFile");
		return 1;
	}
	static int kf_stop(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->sendMain("stop");
		return 1;
	}
	static int kf_help(int c, Fl_Text_Editor* e)
	{
		SCUM_DOCUMENT(e)->sendMain("stop");
		return 1;
	}
	static int kf_selfInsert(int c, Fl_Text_Editor* e)
	{
		char s[1] = { (char)c };
		e->insert(s);
		return 1;
	}
	static int kf_default(int c, Fl_Text_Editor* e)
	{
		SCUM_DEBUG_PRINT("0x%x\n", c);
		return 0;
	}

	struct KeyBindingSpec
	{
		int key;
		int state;
		Fl_Text_Editor::Key_Func func;
	};

	KeyBindingSpec gCommonKeyBindings[] = {
		{ FL_Escape,    FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_ignore     },
		{ FL_Insert,    FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_insert     },
		{ FL_Home,      0,                        Fl_Text_Editor::kf_move       },
		{ FL_End,       0,                        Fl_Text_Editor::kf_move       },
		{ FL_Left,      0,                        Fl_Text_Editor::kf_move       },
		{ FL_Up,        0,                        Fl_Text_Editor::kf_move       },
		{ FL_Right,     0,                        Fl_Text_Editor::kf_move       },
		{ FL_Down,      0,                        Fl_Text_Editor::kf_move       },
		{ FL_Page_Up,   0,                        Fl_Text_Editor::kf_move       },
		{ FL_Page_Down, 0,                        Fl_Text_Editor::kf_move       },
		{ FL_Home,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_End,       FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Left,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Up,        FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Right,     FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Down,      FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Page_Up,   FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Page_Down, FL_SHIFT,                 Fl_Text_Editor::kf_shift_move },
		{ FL_Home,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_End,       FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Left,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Up,        FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Right,     FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Down,      FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Page_Up,   FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Page_Down, FL_CTRL,                  Fl_Text_Editor::kf_ctrl_move  },
		{ FL_Home,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_End,       FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Left,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Up,        FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Right,     FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Down,      FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Page_Up,   FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ FL_Page_Down, FL_CTRL|FL_SHIFT,         Fl_Text_Editor::kf_c_s_move   },
		{ 'z',          FL_COMMAND,               Fl_Text_Editor::kf_undo       },
		{ 'c',          FL_COMMAND,               Fl_Text_Editor::kf_copy       },
		{ 'a',          FL_COMMAND,               Fl_Text_Editor::kf_select_all },

		{ 'n',          FL_CTRL,                  kf_new },
		{ 'o',          FL_CTRL,                  kf_open },
		{ 's',          FL_CTRL,                  kf_save },
		{ 'w',          FL_CTRL,                  kf_close },
		{ 'q',          FL_CTRL,                  kf_quit },

		{ '+',          FL_ALT,                   kf_increaseFontSize },
		{ '-',          FL_ALT,                   kf_decreaseFontSize },

		{ 'x',          FL_ALT,                   kf_executeSelection },
		{ 'y',          FL_ALT,                   kf_methodTemplates },
		{ 'y',          FL_ALT|FL_SHIFT,          kf_methodReferences },
		{ 'j',          FL_ALT,                   kf_openCodeFile },

		{ '.',          FL_ALT,                   kf_stop },
		{ '?',          FL_ALT|FL_SHIFT,          kf_help },

		{ 0,            0,                        0                             }
	};

	KeyBindingSpec gEditKeyBindings [] = {
// 		{ FL_Clear,	  0,                        Fl_Text_Editor::delete_to_eol },
		{ FL_Enter,     FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_enter      },
		{ FL_KP_Enter,  FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_enter      },
		{ FL_BackSpace, FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_backspace  },
		{ FL_Delete,    FL_TEXT_EDITOR_ANY_STATE, Fl_Text_Editor::kf_delete     },
		{ 'x',          FL_COMMAND,               Fl_Text_Editor::kf_cut        },
		{ 'v',          FL_COMMAND,               Fl_Text_Editor::kf_paste      },
		{ '\t',         0,                        kf_selfInsert                 },
		{ 0,            0,                        0                             }
	};

	static void addKeyBindings(Fl_Text_Editor* e, const KeyBindingSpec* src)
	{
		for (int i = 0; src[i].key; i++) {
			e->add_key_binding(src[i].key, src[i].state, src[i].func);
		}
	}

	static int gDocumentCount = 0;
};

SCUM_Document::SCUM_Document(SCUM_DocumentManager* manager)
	: Fl_Text_Editor(0, 0, 0, 0),
	  m_manager(manager),
	  m_editable(false),
	  m_edited(false),
	  m_richText(false)
{
	buffer(new Fl_Text_Buffer());
	cursor_style(CARET_CURSOR);
	textfont(FL_COURIER);
	textsize(12);
	buffer()->tab_distance(4);
	setEditable(true);
	wrap_mode(1, 0);
	Fl::dnd_text_ops(true);
	buffer()->add_modify_callback(bufferModifyCB, this);

	fl_message_icon()->box(FL_NO_BOX);
	fl_message_icon()->label(0);
	fl_message_icon()->image(0);
	fl_message_icon()->labeltype(FL_NO_LABEL);
}

SCUM_Document::~SCUM_Document()
{
	m_manager->remove(this);
	SCUM_InvocationManager::instance().remove(0, this);
	Fl_Text_Buffer* b = buffer();
	buffer(0);
	delete b;
	sendMessage("closed", 0, 0);
}

void SCUM_Document::setName(const std::string& name)
{
	m_name = name;
	setTitle();
}

bool SCUM_Document::isOutput() const
{
	return m_manager->output() == this;
}

Fl_Window* SCUM_Document::makeWindow(int w, int h)
{
	if (!window()) {
		new SCUM_DocumentWindow(this, 400, 400);
		SCUM_ASSERT_PTR(window());
		setTitle();
		if (!inLangContext()) sendMain("addDocument");
	}
	return window();
}

void SCUM_Document::setEditable(bool flag)
{
	if (flag != m_editable) {
		m_editable = flag;
		remove_all_key_bindings();
		SCUM::addKeyBindings(this, SCUM::gCommonKeyBindings);
		if (m_editable) SCUM::addKeyBindings(this, SCUM::gEditKeyBindings);
	}
}

void SCUM_Document::setEdited(bool flag)
{
	if (flag != m_edited) {
		m_edited = flag;
		setTitle();
	}
}

void SCUM_Document::openFile(const char* path)
{
	buffer()->loadfile(path);
	char* txt = buffer()->text();
	if (rtf2txt(txt) > 0) {
		m_path = "";
		m_name = fl_filename_name(path);
		m_richText = true;
		textfont(FL_HELVETICA);
		buffer()->text(txt);
	} else {
		m_path = path;
		m_name = path;
		textfont(FL_COURIER);
	}
	free(txt);
	setEdited(false);
}

bool SCUM_Document::saveToFile()
{
	if (!hasFile()) {
		char* file = fl_file_chooser("save as", 0, m_name.c_str());
		if (!file) return false;
		m_path = file;
		m_name = m_path;
	}
	buffer()->savefile(path().c_str());
	setEdited(false);
	return true;
}

bool SCUM_Document::canClose()
{
	if (isOutput()) return false;
	if (isEdited()) {
		switch (fl_choice("unsaved changes, really close?", "cancel", "save", "close")) {
			case 0: return false;
			case 1: return true;
			case 2: return saveToFile();
		}
	}
	return true;
}

void SCUM_Document::close()
{
	if (window()) window()->do_callback();
}

void SCUM_Document::showWindow()
{
	if (window()) window()->show();
}

char* SCUM_Document::selectionOrLine()
{
	if (buffer()->selected())
		return buffer()->selection_text();;
	return buffer()->line_text(insert_position());	
}

void SCUM_Document::adjustTextSize(int amount)
{
	textsize(textsize() + amount);
	show_insert_position();
	buffer()->call_modify_callbacks();
	redraw();
}

void SCUM_Document::executeSelection()
{
	char* sel = selectionOrLine();
	SCUM_ASSERT_PTR(sel);
	SCUM_FLApp::instance()->setCmdLine(sel);
	SCUM_FLApp::instance()->interpretPrintCmdLine();
	free(sel);
}

void SCUM_Document::sendMain(const char* method, const char* str)
{
	if (compiledOK) {
		SCUM_FLApp::instance()->setCmdLine(str);
		SCUM_FLApp::instance()->runLibrary(method);
	}
}

void SCUM_Document::sendSelection(const char* method)
{
	char* sel = selectionOrLine();
	SCUM_ASSERT_PTR(sel);
	sendMain(method, sel);
	free(sel);
}

void SCUM_Document::setTitle()
{
	SC_StringBuffer buf;

	if (m_name.empty()) {
		buf.appendf("Untitled-%d", ++SCUM::gDocumentCount);
		m_name = std::string(buf.getData(), buf.getSize());
		buf.reset();
	}
	buf.appendf(
		"SuperCollider: %s%s",
		m_name.c_str(),
		m_edited ? " *" : ""
		);
	m_title = std::string(buf.getData(), buf.getSize());

	if (window()) window()->label(m_title.c_str());

	m_manager->updateMenu();
}

int SCUM_Document::handle(int evt)
{
	switch (evt)
	{
		case FL_PUSH:
			if (Fl::event_button() == 3) {
				SCUM_Document* doc = m_manager->menuSelect(Fl::event_x(), Fl::event_y(), "Windows");
				if (doc) doc->showWindow();
				return 1;
			}
			break;
		case FL_FOCUS:
			if (!inLangContext()) sendMessage("didBecomeKey", 0, 0);
			break;
		case FL_UNFOCUS:
			if (!inLangContext()) sendMessage("didResignKey", 0, 0);
			break;
		case FL_PASTE:
			if (!m_editable) return 0;
			break;
		case FL_KEYBOARD:
		{
			if (m_editable) return Fl_Text_Editor::handle_key();
			int key = Fl::event_key();
			Key_Func func = bound_key_function(key, Fl::event_state());
			return func ? (*func)(key, this) : 0;
		}
		break;
		case FL_SHORTCUT:
		{
			SCUM_Document* doc = m_manager->shortcutSelect();
			if (doc) {
				doc->showWindow();
				return 1;
			}
		}
		break;
	}
	return Fl_Text_Editor::handle(evt);
}

SCUM_Document::Invocation* SCUM_Document::makeInvocation(const char* type)
{
	return new Invocation(this, type);
}

void SCUM_Document::bufferModified(int pos, int ni, int nd, int nr, const char* del)
{
	setEdited(ni || nd);
}

void SCUM_Document::execute(Invocation* inv)
{
	std::string& type(inv->type);

	if (type == "close") {
		close();
	} else if (type == "foo") {
	}
}

void SCUM_Document::bufferModifyCB(int pos, int ni, int nd, int nr, const char* del, void* arg)
{
	((SCUM_Document*)arg)->bufferModified(pos, ni, nd, nr, del);
}

// =====================================================================
// SCUM_DocumentManager

SCUM_DocumentManager::SCUM_DocumentManager()
	: m_output(0),
	  m_menu(0)
{
	setOutput(create());
	m_output->setName("Output");
}

SCUM_DocumentManager::~SCUM_DocumentManager()
{
	delete [] m_menu;
}

void SCUM_DocumentManager::setOutput(SCUM_Document* doc)
{
	m_output = doc;
	rebuildMenu();
}

SCUM_Document* SCUM_DocumentManager::create()
{
	SCUM_Document* doc = new SCUM_Document(this);
	add(doc);
	return doc;
}

SCUM_Document* SCUM_DocumentManager::open(const char* path)
{
	SCUM_ASSERT_PTR(path);
	SCUM_Document* doc = create();
	doc->openFile(path);
	return doc;
}

SCUM_Document* SCUM_DocumentManager::open()
{
	char* path = fl_file_chooser("open", 0, 0);
	return path ? open(path) : 0;
}

int SCUM_DocumentManager::index(SCUM_Document* doc)
{
	int index = 0;
	List::iterator it = m_list.begin();
	while (it != m_list.end()) {
		if (*it == doc) return index;
		index++; it++;
	}
	return -1;
}

SCUM_Document* SCUM_DocumentManager::at(int index)
{
	List::iterator it = m_list.begin();
	while ((index > 0) && (it != m_list.end())) {
		index--; it++;
	}
	return it == m_list.end() ? 0 : *it;
}

SCUM_Document* SCUM_DocumentManager::last()
{
	return m_list.empty() ? 0 : m_list.back();
}

SCUM_Document* SCUM_DocumentManager::menuSelect(int x, int y, const char* title)
{
	if (m_menu) {
		const Fl_Menu_Item* item = m_menu->popup(x, y, title);
		if (item) return SCUM_DOCUMENT(item->user_data());
	}
	return 0;
}

SCUM_Document* SCUM_DocumentManager::shortcutSelect()
{
	if (m_menu) {
		const Fl_Menu_Item* item = m_menu->test_shortcut();
		if (item) return SCUM_DOCUMENT(item->user_data());
	}
	return 0;
}

void SCUM_DocumentManager::updateMenu()
{
	if (m_menu) {
		Fl_Menu_Item* item = m_menu;
		List::iterator it = m_list.begin();
		while (item && (it != m_list.end())) {
			item->label((*it)->name().c_str());
			item++; it++;
		}
	}
}

void SCUM_DocumentManager::rebuildMenu()
{
	delete [] m_menu;
	if (m_list.empty()) {
		m_menu = 0;
	} else {
		m_menu = new Fl_Menu_Item[size() + 1];
		memset(m_menu, 0, (size() + 1) * sizeof(Fl_Menu_Item));
		List::iterator it = m_list.begin();
		Fl_Menu_Item* item = m_menu;
		char accel = '1';
		while (it != m_list.end()) {
			SCUM_Document* doc = *it++;
			item->user_data(doc);
			item->label(doc->name().c_str());
			if (doc == output()) item->shortcut(FL_ALT+'0');
			else if (accel <= '9') item->shortcut(FL_ALT+accel++);
			item++;
		}
	}
}

void SCUM_DocumentManager::add(SCUM_Document* doc)
{
	m_list.push_back(doc);
	rebuildMenu();
}

void SCUM_DocumentManager::remove(SCUM_Document* doc)
{
	m_list.remove(doc);
	rebuildMenu();
}

// =====================================================================
// SCUM_DocumentWindow

SCUM_DocumentWindow::SCUM_DocumentWindow(SCUM_Document* view, int w, int h)
	: Fl_Window(w, h)
{
	add(view);
	resizable(view);
	view->resize(0, 0, w, h);
	end();
	callback(&SCUM::Document_closeCB, view);
}

SCUM_DocumentWindow::~SCUM_DocumentWindow()
{
}

// EOF
