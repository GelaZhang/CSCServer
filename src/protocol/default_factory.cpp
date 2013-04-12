#include "default_factory.h"
#include "command_distributor.h"
#include "command/echo_controller.h"
using namespace Net;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new CommandDistributor(diplomat);
}
	/**
	*	\brief 启动协议工厂，内部自动调用， 在此做一些初始化操作
	*/
void DefaultFactory::StartEmbassy() {
	CommandDistributor::RegisterCommand(
		new EchoController("Echo"));

}
	/**
	*	\brief 停止协议工厂，内部自动调用，在此做一些清除操作
	*/
void DefaultFactory::StopEmbassy() {

}