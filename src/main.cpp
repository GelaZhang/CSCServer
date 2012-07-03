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

#include <stdlib.h>


int main(int argc, char* argv[]) {

	Embassy embassy;
	NetService svr(5055);
	svr.Init(&embassy, 2);
	svr.Start();
#ifndef WIN32
	sleep(50);
#else
    Sleep(30000);
#endif
    
	svr.Stop();
	svr.UnInit();
	return 0;
}

