/*
 * diplomat.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */
#include "diplomat.h"
#ifndef WIN32
#include <strings.h>

#include <arpa/inet.h>
#else
#include <string.h>
#endif


#include "event.h"

#include "def.h"
#include "syslog.h"

using namespace std;

Diplomat::Diplomat(struct bufferevent *bev, const string &id,
		struct sockaddr *addr, int len) :
_id(id) {

	_bev = bev;
	_method_num = 0;
	_send_cnt = 0;
	_recv_cnt = 0;
	bzero(_ip, INET_ADDRSTRLEN + 1);
	struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
	evutil_inet_ntop(AF_INET, (void*)(&addr_in->sin_addr), _ip, INET_ADDRSTRLEN);
	_port = ntohs(addr_in->sin_port);
}

Diplomat::~Diplomat() {

	if ( _bev )
		bufferevent_free(_bev);
	_bev = NULL;

}

int Diplomat::Speek(const BYTE *content, int len) {

	AutoMutex auto_mutex(_mutex_speek);
	if ( 0 == bufferevent_write(_bev, content, len) ) {
		_send_cnt += len;
		return kOK;
	}
	return kLibEventErr;
}

size_t Diplomat::GetMassageLength() {

	return evbuffer_get_length(bufferevent_get_input(_bev));
}

size_t Diplomat::GetPeekAbleLength() {

	return evbuffer_get_contiguous_space(bufferevent_get_input(_bev));
}

size_t Diplomat::Peek(void **buffer) {

    struct evbuffer_iovec v = { NULL, 0 };
    evbuffer_peek(bufferevent_get_input(_bev), -1, NULL, &v, 1);
    *buffer = v.iov_base;
    return v.iov_len;
}

size_t Diplomat::ReadMassage(BYTE *content, size_t buffer_length, size_t length_expect) {

	if ( !content )
		return 0;

	if ( 0 == length_expect ) {

		size_t len = evbuffer_get_length(bufferevent_get_input(_bev));
		buffer_length = buffer_length > len ? len : buffer_length;
	}
	size_t cnt = bufferevent_read(_bev, content, buffer_length);
	_recv_cnt += cnt;
	return cnt;
}


void Diplomat::Dump() {
	syslog(LOG_INFO, "%s	%s	%d	%d	%d	%d\n", 
		_id.c_str(), _ip, _port, _method_num, _recv_cnt, _send_cnt);
}