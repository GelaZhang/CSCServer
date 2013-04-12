/**
*  \brief 默认协议工厂
*	date 4 12, 2013
*/

#include "embassy.h"

class DefaultFactory : public Embassy {

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