/* 
 * filename : embassy.h
 * author   : sundayman
 * date     : 4 12, 2013
 * brief    : Э��ӿڣ�Ҫʵ���µ�Э�飬��Ҫ�̳иýӿڣ�������Э�鹤������� 
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/**
 * \brief embassy service for protocol analysis layer.
 * \	  if you want to change default action of the embassy ,
 * \		 you can inherit class Embassy and then implement your version
 *	Э����󹤳��ӿڣ�ʵ������ӿھͿ���ʵ���ض���Э�鹤��������Э�����Ķ���
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
