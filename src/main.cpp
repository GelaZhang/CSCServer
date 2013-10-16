/*
 * main.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */
#ifdef WIN32
#include <WinSock2.h>
#endif
#include <stdlib.h>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "def.h"
#include "net_service.h"
#include "embassy.h"
#include "protocol/default_factory.h"
#include "protocol/command/echo_controller.h"

#pragma comment(lib, "log4cxx.lib")
using namespace CSCServer;
#if _DEBUG
struct State {
	DiplomatPtr diplomat;

};
#endif

int main(int argc, char* argv[]) {

	log4cxx::PropertyConfigurator::configure("log4cxx.properties");

	DefaultFactory embassy;
	
	NetService svr(5055);
	svr.Init(&embassy, 4);
	svr.Start();
	for (;;) {
#if _WIN32
		Sleep(5000);
#else
		sleep(5);
#endif
		svr.Dump();
	}
#ifndef WIN32
	sleep(50);
#else
    Sleep(30000);
#endif
    
	svr.Stop();
	svr.UnInit();
	return 0;
}

