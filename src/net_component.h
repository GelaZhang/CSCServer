/*
 * \filename: net_component.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 6, 2012
 */

#ifndef NET_COMPONENT_H_
#define NET_COMPONENT_H_

class NetComponent {

public:
	/**
	 * \brief 初始化通用服务器
	 */
	int Init();

	/**
	 * \brief 开启通用服务器，必须先Init才能Start
	 */
	int Start();

	/**
	 * \brief 停止通用服务器
	 */
	int Stop();

	/**
	 * \brief 清除通用服务器资源，必须先Stop才能UnInit
	 */
	int UnInit();
};


#endif /* NET_COMPONENT_H_ */
