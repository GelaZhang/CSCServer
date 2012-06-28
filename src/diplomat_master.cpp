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

#include "embassy.h"

using namespace std;
DiplomatMaster::DiplomatMaster() :
_id_master("")	{

	_embassy = NULL;
}

void DiplomatMaster::SetEmbassy(Embassy *embassy) {

	_embassy = embassy;
}

void DiplomatMaster::BuildDiplomat(struct event_base *base,
	    evutil_socket_t sock, struct sockaddr *addr, int len) {

	struct bufferevent *bev = bufferevent_socket_new(base, sock,
			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev ) {
		event_warnx("can't new bufferevent");
		return;
	}
	string id;
	_id_master.GetIdentity(id);
	DiplomatPtr diplomat = new Diplomat(bev, id, addr, len);

	bufferevent_setcb(bev, ReadCB, NULL, EventCB, this);

	{
		AutoMutex auto_mutex(_mutex);
		DiplomatDic::iterator diplomat_itor = _diplomat_dic.find(bev);
		if ( diplomat_itor != _diplomat_dic.end() ) {
			event_msgx("bev has got a diplomat");
			_diplomat_dic.erase(diplomat_itor);
		}
		_diplomat_dic[bev] = diplomat;
	}
	//By default, a newly created bufferevent has writing enabled, but not reading.
	//so enable reading here
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	if ( _embassy ) {
		_embassy->GarrisonDiplomat(diplomat);
	}

	printf("new socket :%s:%d\n", _diplomat_dic[bev]->IP(), _diplomat_dic[bev]->Port());
}

void DiplomatMaster::FreeDiplomat(struct bufferevent *bev) {

	DiplomatPtr diplomat = NULL;
	{
		AutoMutex auto_mutex(_mutex);
		DiplomatDic::iterator diplomat_itor = _diplomat_dic.find(bev);
		if ( diplomat_itor != _diplomat_dic.end() ) {
			diplomat = diplomat_itor->second;
			_diplomat_dic.erase(diplomat_itor);
		}
	}

	string id = diplomat->Id();
	if ( _embassy ) {

		_embassy->WithdrawDiplomat(diplomat);
	}
	_id_master.ReleaseIdentity(id);

	printf("socket close id:%s\n", id.c_str());
}

void DiplomatMaster::ReadCB(struct bufferevent *bev, void *ctx) {

	DiplomatMaster *master = reinterpret_cast<DiplomatMaster*>(ctx);
	assert(master);
	DiplomatPtr diplomat = NULL;
	{
		AutoMutex auto_mutex(master->_mutex);
		DiplomatDic::iterator diplomat_itor = master->_diplomat_dic.find(bev);
		if ( diplomat_itor != master->_diplomat_dic.end() &&  master->_embassy ) {

			diplomat = diplomat_itor->second;
		}
	}
	master->_embassy->RecvSomething(diplomat);
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
