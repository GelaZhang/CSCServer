/*
 * \filename: env_master.cpp
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */


#include "env_master.h"

#include "event.h"

void EnvMaster::SetLogInterface(log_interface interface) {

	event_set_log_callback(interface);
}
