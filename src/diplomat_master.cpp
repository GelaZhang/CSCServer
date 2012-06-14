/*
 * diplomat_master.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */

#include "diplomat_master.h"

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

extern "C" {
#include "log-internal.h"
}

#include "diplomat.h"

void DiplomatMaster::BuildDiplomat(struct event_base *base,
	    evutil_socket_t sock, struct sockaddr *addr, int len) {

	struct bufferevent *bev = bufferevent_socket_new(base, sock,
			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev ) {
		event_warnx("can't new bufferevent");
		return;
	}

	DiplomatDic::iterator diplomat_itor = _diplomat_dic.find(bev);
	if ( diplomat_itor != _diplomat_dic.end() ) {
		event_msgx("bev has got a diplomat");
		if ( diplomat_itor->second ) {
			delete diplomat_itor->second;
			diplomat_itor->second = NULL;
		}
	}
	_diplomat_dic[bev] = new Diplomat(bev);

	bufferevent_setcb(bev, ReadCB, NULL, EventCB, this);
	//By default, a newly created bufferevent has writing enabled, but not reading.
	//so enable reading here
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
	printf("new socket :%s:%d\n", inet_ntoa(addr_in->sin_addr), ntohs(addr_in->sin_port));
}

void DiplomatMaster::FreeDiplomat(struct bufferevent *bev) {

	DiplomatDic::iterator diplomat_itor = _diplomat_dic.find(bev);
	if ( diplomat_itor != _diplomat_dic.end() ) {
		if ( diplomat_itor->second ) {
			delete diplomat_itor->second;
			diplomat_itor->second = NULL;
		}
		_diplomat_dic.erase(diplomat_itor);
	}
	bufferevent_free(bev);
	printf("socket close\n");
}

void DiplomatMaster::ReadCB(struct bufferevent *bev, void *ctx) {
	printf("read something\n");
}

void DiplomatMaster::EventCB(struct bufferevent *bev, short events, void *ctx) {

	DiplomatMaster *master = reinterpret_cast<DiplomatMaster*>(ctx);
	assert(master);

	if (events & BEV_EVENT_ERROR) {

		event_warnx("Error from bufferevent, %s",
				evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()) );
	}
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
    	master->FreeDiplomat(bev);
    }
    printf("buffer event\n");
}
