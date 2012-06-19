/*
 * diplomat.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */
#include "diplomat.h"

#include "event.h"

#include "def.h"

using namespace std;

Diplomat::Diplomat(struct bufferevent *bev, const string &id) :
_id(id) {

	_bev = bev;
}

Diplomat::~Diplomat() {

	if ( _bev )
		bufferevent_free(_bev);
	_bev = NULL;

}

int Diplomat::Speek(const char* content, int len) {

	return kError;
}
