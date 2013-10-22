/*
 * \filename: net_service.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#ifndef NET_SERVICE_H_
#define NET_SERVICE_H_

#include <semaphore.h>
#include "util/thread.h"
#include "diplomat_master.h"

struct event_base;
struct evconnlistener;
namespace CSCServer {
class Embassy;

class NetService {

public:
	NetService(short int net_port);
	~NetService();
	/**
	 * \remark 此函数将阻塞，直到调用Quit或者按下Ctrl-C(发出SIGINT信号)
	 */
	int Main(int argc, char* argv[]);
	/**
	 * \brief 初始化网络服务
	 * \param[in] embassy is the office of the diplomat
	 * \param[in] concurrent_num 并发数，即线程个数
	 */
	int Init(Embassy *embassy, int concurrent_num);
	/**
	 * \brief 退出Main函数
	 */
	void Quit();
protected:


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

	void Dump() {
		_master.Dump();
	}
private:

	static void DealwithNetError(struct evconnlistener *lis, void *ptr);

	static void BuildDiplomat(struct evconnlistener *listener,
    evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr);

private:
    class EventThread : public Utility::Thread {

    public:
        EventThread(event_base *);
        void run();
        
    private:
        event_base *_event_base;
    };
    typedef Utility::SmartPtr<EventThread> EventThreadPtr;
private:
	event_base **_event_base;
	event **_event_hold_base;
	event *_signal_event;
	int _e_base_index;
	int _concurrent_num;
	EventThreadPtr *_event_thread;
	evconnlistener  *_event_listener;
	short int _net_port;
	Embassy *_embassy;
	DiplomatMaster _master;
	sem_t _signal;
};
};

#endif /*NET_SERVICE_H_*/
