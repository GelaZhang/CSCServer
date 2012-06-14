/*
 * \filename: net_service.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#ifndef NET_SERVICE_H_
#define NET_SERVICE_H_

#include "diplomat_master.h"

struct event_base;
struct evconnlistener;

class NetService {

public:
	NetService(short int net_port);
	~NetService();
	/**
	 * \brief 初始化网络服务
	 */
	int Init();

	/**
	 * \brief 开启网络服务，必须先Init才能Start
	 */
	int Start();

	/**
	 * \brief 停止网络服务
	 */
	int Stop();

	/**
	 * \brief 清除网络服务，必须先Stop才能UnInit
	 */
	int UnInit();
private:

	static void DealwithNetError(struct evconnlistener *lis, void *ptr);

	static void BuildDiplomat(struct evconnlistener *listener,
    evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr);

private:
	event_base *_event_base;
	evconnlistener  *_event_listener;
	short int _net_port;

	DiplomatMaster _master;
};


#endif /*NET_SERVICE_H_*/
