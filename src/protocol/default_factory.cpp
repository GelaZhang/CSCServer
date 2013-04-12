#include "default_factory.h"
#include "command_distributor.h"
#include "command/echo_controller.h"
using namespace Net;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new CommandDistributor(diplomat);
}
	/**
	*	\brief ����Э�鹤�����ڲ��Զ����ã� �ڴ���һЩ��ʼ������
	*/
void DefaultFactory::StartEmbassy() {
	CommandDistributor::RegisterCommand(
		new EchoController("Echo"));

}
	/**
	*	\brief ֹͣЭ�鹤�����ڲ��Զ����ã��ڴ���һЩ�������
	*/
void DefaultFactory::StopEmbassy() {

}