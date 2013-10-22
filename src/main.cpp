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
#ifdef _WIN32
#pragma comment(lib, "log4cxx.lib")
#endif
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
	svr.Main(argc, argv);
	return 0;
}

