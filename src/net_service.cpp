/*
 * \filename: NetService.cpp
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */
#ifdef WIN32
#include <WinSock2.h>
#endif
#include "net_service.h"

#include "stddef.h"
#ifndef WIN32
#include "strings.h"
#include <arpa/inet.h>
#else
#include <time.h>
#include <Windows.h>
#endif
#include <string.h>
#include <assert.h>

#include <errno.h>

#include "event.h"
extern "C" {
#include "event2/listener.h"
#include "log-internal.h"
#include "event2/thread.h"
}

#include "def.h"

#include "env_master.h"

using namespace CSCServer;
NetService::NetService(short int net_port) {

	_net_port = net_port;
	_event_base = NULL;
	_event_hold_base = NULL;
	_event_listener = NULL;
	_e_base_index = 0;
	_concurrent_num = 0;
	_event_thread = NULL;
	_embassy = NULL;
}

NetService::~NetService() {

	UnInit();

}

void event_cb_fn(evutil_socket_t, short, void *) {
#if _DEBUG
	printf("event cb fn\n");
#endif
}

int NetService::Init(Embassy *embassy, int concurrent_num) {

	EnvMaster env;
	env.SetLogInterface();
#ifdef WIN32
    WSADATA wsd;
    if ( 0 != WSAStartup(MAKEWORD(2,2), &wsd) )
    {
        return kError;
    }
    if (!( 2 == LOBYTE(wsd.wVersion) && 2 == HIBYTE(wsd.wVersion) ) )
    {
        return kError;
    }
#else
    evthread_use_pthreads();
#endif


    if ( !embassy || concurrent_num <= 0 ) {
    	return kError;
    }
	_embassy = embassy;
	_master.SetEmbassy(embassy);

    _event_base = new event_base*[concurrent_num];
    _concurrent_num = concurrent_num;
    if ( _concurrent_num > 1 )
    	_event_hold_base = new event*[concurrent_num - 1];
    _event_thread = new EventThreadPtr[concurrent_num];
    for ( int i = 0; i < concurrent_num; i ++ ) {

    	_event_base[i] = event_base_new();
        _event_thread[i] = new EventThread(_event_base[i]);
        event_msgx("base %d : %d", i, _event_base[i]);
    }

    struct timeval timeout = {2,1};
    for ( int i = 0; i < concurrent_num - 1; i ++ ) {

    	_event_hold_base[i] = event_new(_event_base[i+1], -1, EV_PERSIST|EV_WRITE, event_cb_fn, NULL );
    	event_add(_event_hold_base[i], &timeout);
    }
    

    sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(_net_port);

    _event_listener = evconnlistener_new_bind(_event_base[_e_base_index], NetService::BuildDiplomat, this,
        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    if (!_event_listener) {
    	event_warnx("Couldn't create listener");
        return kLibEventErr;
    }
    evconnlistener_set_error_cb(_event_listener, NetService::DealwithNetError);

	
    _e_base_index = (_e_base_index + 1) % _concurrent_num;
	return kOK;
}

int NetService::Start() {
	_embassy->StartEmbassy();
	for ( int i = 0; i < _concurrent_num; i ++ ) {
		_event_thread[i]->start();
	}

	return kOK;
}

int NetService::Stop() {

	event_msgx("net service stopping");

    if ( _event_listener ) {
    	evconnlistener_disable(_event_listener);
        event_msgx("disable listener");
    }

    if ( _event_hold_base ) {

        for ( int i = 0; i < _concurrent_num - 1; i ++ ) {

            event_del(_event_hold_base[i]);
            event_msgx("del timeout event");
        }
    }
    _master.FreeAllDiplomat();
    for ( int i = 0; i < _concurrent_num; i ++ ) {

    	event_msgx("ready to exit %d", _event_base[i]);
    	//event_base_loopexit(_event_base[i], NULL);
    	event_base_loopbreak(_event_base[i]);
	}
    event_msgx("ready to join thread");
	for ( int i = 0; i < _concurrent_num; i ++ ) {
		_event_thread[i]->getThreadControl().join();

	}
	_embassy->StopEmbassy();
	event_msgx("net service stop");
	return kOK;
}

int NetService::UnInit() {


    if ( _event_listener ) {
        evconnlistener_free(_event_listener);
        event_msgx("free listener");
        _event_listener = NULL;
    }

	if ( _event_hold_base ) {

		for ( int i = 0; i < _concurrent_num - 1; i ++ ) {

			event_free(_event_hold_base[i]);
		}
		delete []_event_hold_base;
		_event_hold_base = NULL;
	}

	if ( _event_thread ) {
		delete []_event_thread;
		_event_thread = NULL;
	}

	if ( _event_base ) {

		for ( int i = 0 ; i < _concurrent_num; i ++ ) {

			event_base_free(_event_base[i]);
		}
		delete []_event_base;
		_event_base = NULL;
	}

	_e_base_index = 0;
#ifdef WIN32
    if ( 0 != WSACleanup() )
    {
        int errCode = WSAGetLastError();
        //����log
    }
#endif
	return kOK;
}

void NetService::DealwithNetError(struct evconnlistener *lis, void *ptr) {

	NetService *service = reinterpret_cast<NetService*>(ptr);
	assert(service);

    int err = EVUTIL_SOCKET_ERROR();
    event_warnx("Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));


}

void NetService::BuildDiplomat(struct evconnlistener *listener,
	    evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr) {

	NetService *service = reinterpret_cast<NetService*>(ptr);
	assert(service);
	service->_master.BuildDiplomat(service->_event_base[service->_e_base_index], sock, addr, len);
	service->_e_base_index = (service->_e_base_index + 1) % service->_concurrent_num;
}

NetService::EventThread::EventThread(event_base *base) {
    _event_base = base;
}

void NetService::EventThread::run() {
	event_msgx("base %d\n", _event_base);
    event_base_dispatch(_event_base);
    event_msgx("thread finish base %d\n", _event_base);
}
