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


#include "SCUM_FLApp.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Prim.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"
#include "SCUM_ViewFactory.hh"

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <FL/Fl.H>

#include <fcntl.h>
#include <unistd.h>

#include <SC_StringBuffer.h>
#include <GC.h>
#include <PyrKernel.h>
#include <PyrObject.h>
#include <VMGlobals.h>

const float SCUM_TICK_INTERVAL = 0.05;

// =====================================================================
// SCUM_FLApp

SCUM_FLApp::SCUM_FLApp(const char* name)
	: SC_TerminalClient(name)
{ }

SCUM_FLApp::~SCUM_FLApp()
{ }

void SCUM_FLApp::commandLoop()
{
	// make stdin non-blocking
	const int fd = 0;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0) {
		perror(getName());
		exit(1);
	}

	Fl::add_fd(fd, inputAvailableCB, this);

	daemonLoop();
}

void SCUM_FLApp::daemonLoop()
{
	Fl::add_timeout(0.0, tickTimeoutCB, this);
	while (shouldBeRunning()) Fl::wait();
}

void SCUM_FLApp::onInitRuntime()
{
	SC_TerminalClient::onInitRuntime();
	SCUM::init();
	SCUM_ViewFactory::init();
}

void SCUM_FLApp::onLibraryStartup()
{
	SC_TerminalClient::onLibraryStartup();
	SCUM_Symbol::init();
	SCUM_Prim::init();
}

void SCUM_FLApp::onLibraryShutdown()
{
	SC_TerminalClient::onLibraryShutdown();
	SCUM_Desktop::instance().destroy(true);
}

void SCUM_FLApp::onInterpStartup()
{
	SC_TerminalClient::onInterpStartup();

	// set SCUM.argv
	// TODO: move this to SC_LanguageClient and add Process.argv class
	// variable
	struct VMGlobals* g = getVMGlobals();
	PyrClass* klass;
	int cvx = classVarOffset("SCUM", "argv", &klass);
	if (cvx >= 0) {
		//PyrSlot* argvSlot = g->classvars[klass->classIndex.ui].uo->slots + cvx;
		PyrSlot* argvSlot = &g->classvars->slots[cvx];
		PyrObject* argv = newPyrArray(g->gc, options().mArgc * sizeof(PyrObject), 0, true);
		SetObject(argvSlot, argv);
		for (int i=0; i < options().mArgc; i++) {
			PyrString* str = newPyrString(g->gc, options().mArgv[i], 0, true);
			SetObject(argv->slots+i, str);
			argv->size++;
			g->gc->GCWrite(argv, (PyrObject*)str);
		}
	}
}

void SCUM_FLApp::inputAvailable(int fd)
{
	while (readCmdLine(fd, m_inputBuffer));
}

void SCUM_FLApp::tickTimeoutCB(void* arg)
{
	((SCUM_FLApp*)arg)->tick();
	Fl::repeat_timeout(SCUM_TICK_INTERVAL, tickTimeoutCB, arg);
}

void SCUM_FLApp::inputAvailableCB(int fd, void* arg)
{
	((SCUM_FLApp*)arg)->inputAvailable(fd);
}

int main(int argc, char** argv)
{
	SCUM_FLApp app(PACKAGE);
	return app.run(argc, argv);
}

#if 0
void SCUM_FLApp::inputAvailable(int fd)
{
	const int bufSize = 256;
	char buf[bufSize];
	int n;
	char cmd;
	int32_t size;

	n = read(fd, &cmd, 1);
	if (n != 1) {
		if (n == 0) {
			quit(0);
		} else {
			if (n == -1) perror("SCUM_FLApp");
			quit(1);
		}
		return;
	}

	n = read(fd, &size, sizeof(size));
	if (n != sizeof(size)) {
		if (n == 0) {
			quit(0);
		} else {
			if (n == -1) perror("SCUM_FLApp");
			quit(1);
		}
		return;
	}

	size = ntohl(size);
	while (size > 0) {
		n = read(fd, buf, SCUM::min(size, bufSize));
		if (n > 0) {
			m_inputBuffer.append(buf, n);
			size -= n;
		} else {
			if (n == 0) {
				quit(0);
			} else {
				 if (n == -1) perror("SCUM_FLApp");
				 quit(1);
			}
			return;
		}
	}

	switch (cmd) {
		case kInterpretCmdLine:
			setCmdLine(m_inputBuffer);
			runLibrary(SCUM_Symbol::interpretCmdLine);
			flushPostBuf();
			break;
		case kInterpretPrintCmdLine:
			setCmdLine(m_inputBuffer);
			runLibrary(SCUM_Symbol::interpretPrintCmdLine);
			flushPostBuf();
			break;
		case kRecompileLibrary:
			flushPostBuf();
			recompileLibrary();
			break;
		default:
			::error("invalid command %d in input handler\n", cmd);
			flushPostBuf();
	}

	m_inputBuffer.reset();
}
#endif // 0

// EOF
