/*
 * diplomat_master.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */

#include "diplomat_master.h"

#include <tchar.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif
extern "C" {
#include "log-internal.h"
}

#include "def.h"
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

	bufferevent_setwatermark(bev, EV_READ, 40, 64*1024);
	bufferevent_setcb(bev, ReadCB, NULL, EventCB, this);
	//By default, a newly created bufferevent has writing enabled, but not reading.
	//so enable reading here
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	if ( _embassy ) {
		ProtocolPtr pro = _embassy->BuildProtocol(diplomat);
		pro->GarrisonDiplomat();
		AutoMutex auto_mutex(_mutex);
		ProtocolDic::iterator protocol_itor = _protocol_dic.find(bev);
		if ( protocol_itor != _protocol_dic.end() ) {
			event_msgx("bev has got a diplomat");
			_protocol_dic.erase(protocol_itor);
		}
		_protocol_dic[bev] = pro;
	}
	printf("new socket :%s:%d\n", diplomat->IP(), diplomat->Port());
}

void DiplomatMaster::FreeAllDiplomat() {

    AutoMutex auto_mutex(_mutex);
    for ( ProtocolDic::iterator protocol_itor = _protocol_dic.begin(); 
        protocol_itor != _protocol_dic.end(); ++ protocol_itor ) {
        
            if ( protocol_itor->second ) {

                protocol_itor->second->WithdrawDiplomat();
				string id = protocol_itor->second->GetDiplomat()->Id();
				_id_master.ReleaseIdentity(id);
				printf("socket close id:%s\n", id.c_str());
			}
            
    }
    _protocol_dic.clear();
}

void DiplomatMaster::FreeDiplomat(struct bufferevent *bev) {

	ProtocolPtr pro = NULL;
	{
		AutoMutex auto_mutex(_mutex);
		ProtocolDic::iterator protocol_itor = _protocol_dic.find(bev);
		if ( protocol_itor != _protocol_dic.end() ) {
			pro = protocol_itor->second;
			_protocol_dic.erase(protocol_itor);
		}
	}

	if ( pro ) {

		pro->WithdrawDiplomat();
		string id = pro->GetDiplomat()->Id();
		_id_master.ReleaseIdentity(id);

		printf("socket close id:%s\n", id.c_str());
	}

}

void DiplomatMaster::ReadCB(struct bufferevent *bev, void *ctx) {

	DiplomatMaster *master = reinterpret_cast<DiplomatMaster*>(ctx);
	assert(master);
	ProtocolPtr pro = NULL;
	{
		AutoMutex auto_mutex(master->_mutex);
		ProtocolDic::iterator protocol_itor = master->_protocol_dic.find(bev);
		if ( protocol_itor != master->_protocol_dic.end() &&  master->_embassy ) {

			pro = protocol_itor->second;
		}
	}
    if ( !pro ) {
        event_msgx("bev had been free, abandon recv data");
        return;
    }
	int cnt = pro->RecvSomething();
	pro->GetDiplomat()->GetMethod(cnt);
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
#if _DEBUG
    printf("buffer event\n");
#endif
}

void DiplomatMaster::Dump() {
	AppLog(APP_LOG_INFO, _T("ID	IP	PORT	METHOD	RECV	SEND\n"));
	AutoMutex auto_mutex(_mutex);
	for ( ProtocolDic::iterator itor = _protocol_dic.begin(); itor != _protocol_dic.end();
		++ itor) {
			itor->second->GetDiplomat()->Dump();
	}
}
