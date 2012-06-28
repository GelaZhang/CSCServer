/*
 * filename : embassy.h
 * author   : sundayman
 * date     : Jun 18, 2012
 * brief    :
 */

#include "embassy.h"

#include <stdio.h>

#include "def.h"

int Embassy::GarrisonDiplomat(const DiplomatPtr &diplomat) {

	printf("diplomat %s garrison", diplomat->Id().c_str());
	return kError;
}

int Embassy::WithdrawDiplomat(const DiplomatPtr &diplomat) {

	printf("diplomat %s withdraw", diplomat->Id().c_str());
	return kError;
}

int Embassy::RecvSomething(const DiplomatPtr &diplomat) {
	printf("diplomat %s recv something", diplomat->Id().c_str());
	//diplomat->Speek("read something\n", 15);
	return kError;
}

