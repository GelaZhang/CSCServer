/**
*  \brief Ĭ��Э�鹤��
*	date 4 12, 2013
*/

#include "embassy.h"

class DefaultFactory : public Embassy {

		/**
	*	\brief ����Э�����,һ�����Ӷ�Ӧһ��Э�����
	*/
	virtual ProtocolPtr BuildProtocol(const DiplomatPtr &diplomat);
	/**
	*	\brief ����Э�鹤�����ڲ��Զ����ã� �ڴ���һЩ��ʼ������
	*/
	virtual void StartEmbassy();
	/**
	*	\brief ֹͣЭ�鹤�����ڲ��Զ����ã��ڴ���һЩ�������
	*/
	virtual void StopEmbassy();
};