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

	$Id: SCUM_FLApp.hh,v 1.2 2004/08/15 14:42:23 steve Exp $
*/


#ifndef SCUM_FLAPP_HH_INCLUDED
#define SCUM_FLAPP_HH_INCLUDED

#include "SCUM_Editor.hh"

#include <stdint.h>
#include <SC_TerminalClient.h>

// =====================================================================
// SCUM_FLApp

class SCUM_FLApp : public SC_TerminalClient
{
public:
	SCUM_FLApp(const char* name);
	virtual ~SCUM_FLApp();

	static SCUM_FLApp* instance()
	{
		return (SCUM_FLApp*)SC_LanguageClient::instance();
	}

	inline SCUM_DocumentManager* documentManager() { return &m_documentManager; }
// 	inline SCUM_Document* postView() { return m_postView; }
// 	inline void setPostView(SCUM_Document* view) { m_postView = view; }

	virtual void post(const char *fmt, va_list ap, bool error);
	virtual void post(char c);
	virtual void post(const char* str, size_t len);
	virtual void flush();

protected:
	virtual void onInitRuntime();
	virtual void onLibraryStartup();
	virtual void onLibraryShutdown();
	virtual void onInterpStartup();

	virtual void commandLoop();
	virtual void daemonLoop();

private:
	void inputAvailable(int fd);
	void postDataAvailable(int fd);

	static void tickTimeoutCB(void* arg);
	static void postDataAvailableCB(int fd, void* arg);
	static void inputAvailableCB(int fd, void* arg);

private:
	SCUM_DocumentManager		m_documentManager;
	int							m_argc;
	char**						m_argv;
	SC_StringBuffer				m_inputBuffer;
	int							m_postFifo[2];
	SC_StringBuffer				m_postBuffer;
	pthread_mutex_t				m_postMutex;
};

#endif // SCUM_FLAPP_HH_INCLUDED
