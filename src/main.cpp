/*
 * main.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */
#ifdef WIN32
#include <WinSock2.h>
#endif
#include "net_service.h"

#include "embassy.h"
#include "syslog.h"
#include "protocol/command_distributor.h"
#include "protocol/command/echo_controller.h"
#include <stdlib.h>

using namespace Net;
#if _DEBUG
struct State {
	DiplomatPtr diplomat;

};
#endif

int main(int argc, char* argv[]) {

	CommandDistributor embassy;
	CommandDistributor::RegisterCommand(
		new EchoController("Echo"));

	NetService svr(5055);
	svr.Init(&embassy, 4);
	svr.Start();
	for (;;) {
		Sleep(5000);
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

