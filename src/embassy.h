/* 
 * filename : embassy.h
 * author   : sundayman
 * date     : Jun 15, 2012
 * brief    :
 */

#ifndef EMBASSY_H_
#define EMBASSY_H_

/**
 * \brief embassy service for protocol analysis layer.
 * \	  if you want to change default action of the embassy ,
 * \		 you can inherit class Embassy and then implement your version
 *	协议对象工厂接口，实现这个接口就可以实现特定的协议工厂（产生协议对象的对象）
 */

#include "diplomat.h"
#include "protocol.h"

class Embassy {

public:
	virtual ~Embassy(){}
	/**
	*	\brief 产生协议对象,一个连接对应一个协议对象
	*/
	virtual ProtocolPtr BuildProtocol(const DiplomatPtr &diplomat);
	/**
	*	\brief 启动协议工厂，内部自动调用， 在此做一些初始化操作
	*/
	virtual void StartEmbassy();
	/**
	*	\brief 停止协议工厂，内部自动调用，在此做一些清除操作
	*/
	virtual void StopEmbassy();
};


#endif /* EMBASSY_H_ */
