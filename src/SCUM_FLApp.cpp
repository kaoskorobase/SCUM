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

	$Id: SCUM_FLApp.cpp,v 1.2 2004/08/15 14:42:23 steve Exp $
*/


#include "SCUM_FLApp.hh"
#include "SCUM_Desktop.hh"
#include "SCUM_Editor.hh"
#include "SCUM_GC.hh"
#include "SCUM_Prim.hh"
#include "SCUM_Symbol.hh"
#include "SCUM_System.hh"
#include "SCUM_Util.hh"
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


// =====================================================================
// SCUM_FLApp

namespace SCUM
{
	static const float kTickInterval = 0.005;
	extern void showSplash(const char*, double);
};

SCUM_FLApp::SCUM_FLApp(const char* name)
	: SC_TerminalClient(name)
{
	if (pipe(m_postFifo) == -1) {
		perror(getName());
		exit(1);
	}
	if (fcntl(m_postFifo[0], F_SETFL, O_NONBLOCK) != 0) {
		perror(getName());
		exit(1);
	}
	if (fcntl(m_postFifo[1], F_SETFL, O_NONBLOCK) != 0) {
		perror(getName());
		exit(1);
	}
	pthread_mutex_init(&m_postMutex, 0);
	Fl::add_fd(m_postFifo[0], &postDataAvailableCB, this);

	m_documentManager.output()->makeWindow()->show();
}

SCUM_FLApp::~SCUM_FLApp()
{
}

void SCUM_FLApp::commandLoop()
{
	// make stdin non-blocking
	const int fd = 0;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0) {
		perror(getName());
		exit(1);
	}

	Fl::add_fd(fd, inputAvailableCB, this);

// 	SCUM::showSplash("SCUM", 0);

	daemonLoop();
}

void SCUM_FLApp::daemonLoop()
{
	Fl::add_timeout(0.0, tickTimeoutCB, this);
	while (shouldBeRunning()) Fl::wait();
}

void SCUM_FLApp::post(const char *fmt, va_list ap, bool error)
{
	pthread_mutex_lock(&m_postMutex);
	m_postBuffer.vappendf(fmt, ap);
	pthread_mutex_unlock(&m_postMutex);
}

void SCUM_FLApp::post(char c)
{
	pthread_mutex_lock(&m_postMutex);
	m_postBuffer.append(c);
	pthread_mutex_unlock(&m_postMutex);
}

void SCUM_FLApp::post(const char* str, size_t len)
{
	pthread_mutex_lock(&m_postMutex);
	m_postBuffer.append(str, len);
	pthread_mutex_unlock(&m_postMutex);
}

void SCUM_FLApp::flush()
{
// 	SCUM_DEBUG_PRINT("0\n");
	pthread_mutex_lock(&m_postMutex);
// 	SCUM_DEBUG_PRINT("1\n");
	if (!m_postBuffer.isEmpty()) {
// 		SCUM_DEBUG_PRINT("2\n");
		write(m_postFifo[1], m_postBuffer.getData(), m_postBuffer.getSize());
// 		SCUM_DEBUG_PRINT("3\n");
		m_postBuffer.reset();
	}
// 	SCUM_DEBUG_PRINT("4\n");
	pthread_mutex_unlock(&m_postMutex);	
// 	SCUM_DEBUG_PRINT("5\n");
}

void SCUM_FLApp::onInitRuntime()
{
	SCUM::GCInit();
	SC_TerminalClient::onInitRuntime();
	SCUM_ViewFactory::init();
}

void SCUM_FLApp::onLibraryStartup()
{
	SC_TerminalClient::onLibraryStartup();
	SCUM_Symbol::init();
	SCUM_Prim::init();
	SCUM_Prim::initDocument();
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
		PyrSlot* argvSlot = g->classvars[klass->classIndex.ui].uo->slots + cvx;
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

void SCUM_FLApp::postDataAvailable(int fd)
{
	char buffer[1024];

	SCUM_Document* output = documentManager()->output();

	while (true) {
		int n = read(fd, buffer, 1023);
		if (n > 0) {
			buffer[n] = 0;
			if (output) {
				output->insert(buffer);
				output->show_insert_position();
			}
		} else {
			if ((n == 0) || ((n == -1) && (errno != EAGAIN)))
				quit(1);
			break;
		}
	}
}

void SCUM_FLApp::tickTimeoutCB(void* arg)
{
	((SCUM_FLApp*)arg)->tick();
	Fl::repeat_timeout(SCUM::kTickInterval, tickTimeoutCB, arg);
}

void SCUM_FLApp::inputAvailableCB(int fd, void* arg)
{
	((SCUM_FLApp*)arg)->inputAvailable(fd);
}

void SCUM_FLApp::postDataAvailableCB(int fd, void* arg)
{
	((SCUM_FLApp*)arg)->postDataAvailable(fd);
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
