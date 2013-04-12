/*
 * filename : embassy.h
 * author   : sundayman
 * date     : 4 12, 2013
 * brief    :
 */

#include "embassy.h"

#include <stdio.h>

#include "def.h"

int Protocol::GarrisonDiplomat() {

	printf("diplomat %s garrison", _diplomat->Id().c_str());
	return kError;
}

int Protocol::WithdrawDiplomat() {

	printf("diplomat %s withdraw", _diplomat->Id().c_str());
	return kError;
}

int Protocol::RecvSomething() {
	printf("diplomat %s recv something", _diplomat->Id().c_str());
	//diplomat->Speek("read something\n", 15);
   
	return kError;
}

