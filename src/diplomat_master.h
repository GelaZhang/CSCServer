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

#include "diplomat.h"
#include "util/identity_master.h"
#include "embassy.h"

class DiplomatMaster {

	typedef std::map<bufferevent *, DiplomatPtr> DiplomatDic;
public:

	DiplomatMaster();

	void SetEmbassy(Embassy *embassy);

	void BuildDiplomat(struct event_base *base,
    evutil_socket_t sock, struct sockaddr *addr, int len);

private:
	void FreeDiplomat(struct bufferevent *bev);
private:
	static void ReadCB(struct bufferevent *bev, void *ctx);

	static void EventCB(struct bufferevent *bev, short events, void *ctx);


private:
	DiplomatDic _diplomat_dic;
	Embassy *_embassy;
	IdentityMaster _id_master;

	Mutex _mutex;

};

#endif /* DIPLOMAT_MASTER_H_ */
