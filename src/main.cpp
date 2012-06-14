/*
 * main.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */

#include "net_service.h"

int main(int argc, char* argv[]) {

	NetService svr(5054);
	svr.Init();
	svr.Start();
	return 0;
}

