/*
 * filename : embassy.h
 * author   : sundayman
 * date     : Jun 18, 2012
 * brief    :
 */

#include "embassy.h"

#include <stdio.h>

#include "def.h"

using namespace CSCServer;
ProtocolPtr Embassy::BuildProtocol(const DiplomatPtr &diplomat) {

	 
	printf("diplomat %s garrison", diplomat->Id().c_str());
	return new Protocol(diplomat);
}

void Embassy::StartEmbassy() {

	printf("Start Embassy");
}

void Embassy::StopEmbassy() {
	printf("Stop Embassy");
   
}

