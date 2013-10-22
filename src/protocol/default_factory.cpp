#include "default_factory.h"
#include "default_protocol.h"
#include "command/echo_controller.h"
using namespace CSCServer;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new DefaultProtocol(diplomat);
}
	/**
	*	\brief 启动协议工厂，内部自动调用， 在此做一些初始化操作
	*/
void DefaultFactory::StartEmbassy() {
	DefaultProtocol::RegisterCommand(
		new EchoController("Echo"));

}
	/**
	*	\brief ֹͣ停止协议工厂，内部自动调用，在此做一些清除操作
	*/
void DefaultFactory::StopEmbassy() {

}
