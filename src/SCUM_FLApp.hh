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

	$Id: SCUM_FLApp.hh,v 1.1 2004/07/30 16:20:14 steve Exp $
*/


#ifndef SCUM_FLAPP_HH_INCLUDED
#define SCUM_FLAPP_HH_INCLUDED

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

protected:
	virtual void onInitRuntime();
	virtual void onLibraryStartup();
	virtual void onLibraryShutdown();
	virtual void onInterpStartup();

	virtual void commandLoop();
	virtual void daemonLoop();

private:
	void inputAvailable(int fd);

	static void tickTimeoutCB(void* arg);
	static void inputAvailableCB(int fd, void* arg);

private:
	int							m_argc;
	char**						m_argv;
	bool						m_shouldBeRunning;
	int							m_returnCode;
	SC_StringBuffer				m_inputBuffer;
};

#endif // SCUM_FLAPP_HH_INCLUDED
