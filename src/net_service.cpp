/*
 * \filename: NetService.cpp
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#include "net_service.h"

#include "stddef.h"
#include "strings.h"
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>

#include "event.h"
extern "C" {
#include "event2/listener.h"
#include "log-internal.h"
}

#include "def.h"

NetService::NetService(short int net_port) {

	_net_port = net_port;
	_event_base = NULL;
	_event_hold_base = NULL;
	_event_listener = NULL;
	_e_base_index = 0;
	_concurrent_num = 0;
	_event_thread = NULL;
}

NetService::~NetService() {

	UnInit();

}

void event_cb_fn(evutil_socket_t, short, void *) {
	printf("event cb fn\n");
}

int NetService::Init(Embassy *embassy, int concurrent_num) {

    if ( !embassy || concurrent_num <= 0 ) {
    	return kError;
    }

	_master.SetEmbassy(embassy);

    _event_base = new event_base*[concurrent_num];
    _concurrent_num = concurrent_num;
    if ( _concurrent_num > 1 )
    	_event_hold_base = new event*[concurrent_num - 1];
    for ( int i = 0; i < concurrent_num; i ++ ) {

    	_event_base[i] = event_base_new();
    }
    struct timeval timeout = {time(0),0};
    for ( int i = 0; i < concurrent_num - 1; i ++ ) {

    	_event_hold_base[i] = event_new(_event_base[i+1], -1, EV_PERSIST, event_cb_fn, NULL);
    	event_add(_event_hold_base[i], &timeout);
    }
    _event_thread = new pthread_t[concurrent_num];

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

	for ( int i = 0; i < _concurrent_num; i ++ ) {
		pthread_create(&_event_thread[i], NULL, event_loop, (void*)(_event_base[i]));
	}

	return kOK;
}

int NetService::Stop() {

	for ( int i = 0; i < _concurrent_num; i ++ ) {

		event_base_loopexit(_event_base[i], NULL);
	}

	for ( int i = 0; i < _concurrent_num; i ++ ) {
		pthread_join(_event_thread[i], NULL);
	}

	return kOK;
}

int NetService::UnInit() {

	if ( _event_listener ) {

		evconnlistener_free(_event_listener);
		_event_listener = NULL;
	}

	if ( _event_base ) {

		for ( int i = 0 ; i < _concurrent_num; i ++ ) {

			event_base_free(_event_base[i]);
		}
		_event_base = NULL;
	}

	if ( _event_hold_base ) {

		for ( int i = 0; i < _concurrent_num - 1; i ++ ) {

			event_free(_event_hold_base[i]);
		}
		_event_hold_base = NULL;
	}

	if ( _event_thread ) {
		delete []_event_thread;
		_event_thread = NULL;
	}
	_e_base_index = 0;

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

void* NetService::event_loop(void *arg) {

	printf("event_loop start %d\n", (int)arg);
	event_base_dispatch(reinterpret_cast<event_base*>(arg) );
	printf("thread quit %d \n", (int)arg);
	return NULL;
}

