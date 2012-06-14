/*
 * \filename: diplomat_master.h
 * \brief	: responsibility for manage socket, in another word it is the master of diplomat
 * \author	: sundayman
 * \date	: Jun 12, 2012
 */

#ifndef DIPLOMAT_MASTER_H_
#define DIPLOMAT_MASTER_H_

#include <map>
#include "event.h"

class Diplomat;
class DiplomatMaster {

	typedef std::map<bufferevent *, Diplomat*> DiplomatDic;
public:
	void BuildDiplomat(struct event_base *base,
    evutil_socket_t sock, struct sockaddr *addr, int len);

private:
	void FreeDiplomat(struct bufferevent *bev);
private:
	static void ReadCB(struct bufferevent *bev, void *ctx);

	static void EventCB(struct bufferevent *bev, short events, void *ctx);

private:
	DiplomatDic _diplomat_dic;

};

#endif /* DIPLOMAT_MASTER_H_ */
