/* 
 * filename : protocol.h
 * author   : sundayman
 * date     : 4 12, 2013
 * brief    : 协议接口，要实现新的协议，需要继承该接口，并且由协议工厂类调用
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/**
 * \brief 协议接口基类，实现新的协议解析，需要实现该接口.
 */

#include "diplomat.h"
namespace CSCServer {
class Protocol : public Utility::RefCounter {
public:
	Protocol(const DiplomatPtr &diplomat) {
		_diplomat = diplomat;
	}
	virtual ~Protocol(){}
	virtual int GarrisonDiplomat();
	virtual int WithdrawDiplomat();
	virtual int RecvSomething();
	DiplomatPtr GetDiplomat() {
		return _diplomat;
	}
protected:
	DiplomatPtr _diplomat;
};
typedef Utility::SmartPtr<Protocol> ProtocolPtr;
};
#endif /* PROTOCOL_H_ */
