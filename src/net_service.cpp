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
	_event_base = event_base_new();
	_event_listener = NULL;
}

NetService::~NetService() {

	if ( _event_base )
		event_base_free(_event_base);
	if ( _event_listener )
		evconnlistener_free(_event_listener);
}

int NetService::Init(Embassy *embassy) {

	_master.SetEmbassy(embassy);

    if (!_event_base) {
    	return kLibEventErr;
    }

    sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(_net_port);

    _event_listener = evconnlistener_new_bind(_event_base, NetService::BuildDiplomat, this,
        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    if (!_event_listener) {
    	event_warnx("Couldn't create listener");
        return kLibEventErr;
    }
    evconnlistener_set_error_cb(_event_listener, NetService::DealwithNetError);

	return kOK;
}

int NetService::Start() {

	event_base_dispatch(_event_base);
	return kOK;
}

void NetService::DealwithNetError(struct evconnlistener *lis, void *ptr) {

	NetService *service = reinterpret_cast<NetService*>(ptr);
	assert(service);

    int err = EVUTIL_SOCKET_ERROR();
    event_warnx("Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(service->_event_base, NULL);
}

void NetService::BuildDiplomat(struct evconnlistener *listener,
	    evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr) {

	NetService *service = reinterpret_cast<NetService*>(ptr);
	assert(service);
	service->_master.BuildDiplomat(service->_event_base, sock, addr, len);
}


