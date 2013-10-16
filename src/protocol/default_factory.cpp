#include "default_factory.h"
#include "default_protocol.h"
#include "command/echo_controller.h"
using namespace CSCServer;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new DefaultProtocol(diplomat);
}
	/**
	*	\brief ����Э�鹤�����ڲ��Զ����ã� �ڴ���һЩ��ʼ������
	*/
void DefaultFactory::StartEmbassy() {
	DefaultProtocol::RegisterCommand(
		new EchoController("Echo"));

}
	/**
	*	\brief ֹͣЭ�鹤�����ڲ��Զ����ã��ڴ���һЩ�������
	*/
void DefaultFactory::StopEmbassy() {

}