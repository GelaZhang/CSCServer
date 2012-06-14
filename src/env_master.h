/*
 * \filename: env_master.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#ifndef ENV_MASTER_H_
#define ENV_MASTER_H_

typedef void (*log_interface)(int severity, const char *msg);

class EnvMaster {

public:
	void SetLogInterface(log_interface interface);
};


#endif /* ENV_MASTER_H_ */
