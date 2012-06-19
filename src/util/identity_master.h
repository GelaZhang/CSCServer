/* 
 * filename : identity_master.h
 * author   : sundayman
 * date     : Jun 18, 2012
 * brief    : 
 */

#ifndef IDENTITY_MASTER_H_
#define IDENTITY_MASTER_H_

#include <string>
#include <map>

#include "mutex.h"

class IdentityMaster {

public:
	/**
	*	\brief  如果key为NULL则默认前缀为identity
	*	\param[in] key 统一名称的前缀,如果key为：identity,则从此处得到名字形式如下：identity1,identity2....
	*/
	IdentityMaster(const char *key = NULL);
	/**
	*	\brief 有名称需要向管理器汇报，才不会产生重复内容。
	*/
	bool AddIdentity(const char *identity);
	/**
	*	\brief 产生一个名称。
	*/
	bool GetIdentity(std::string &identity);
	/**
	*	\brief 释放一个名称。
	*/
	bool ReleaseIdentity(const std::string &identity);
protected:
	typedef std::map<unsigned long, char> MapInt2Char;
	unsigned long _index;							//未分配的名称。连续的
	std::string _key;						//前缀
	MapInt2Char _map_idle_identity;//空闲名称列表。不连续的,小于m_index的
	MapInt2Char _map_using_identity;//正在使用的零散的名称,在m_index右侧（大于）的

	Mutex _mutex;
};


#endif /* IDENTITY_MASTER_H_ */
