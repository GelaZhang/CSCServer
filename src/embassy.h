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
 *	Э����󹤳��ӿڣ�ʵ������ӿھͿ���ʵ���ض���Э�鹤��������Э�����Ķ���
 */

#include "diplomat.h"
#include "protocol.h"

class Embassy {

public:
	virtual ~Embassy(){}
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


#endif /* EMBASSY_H_ */
