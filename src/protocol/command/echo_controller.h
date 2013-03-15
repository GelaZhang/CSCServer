/* 
 * filename : sys_time_controller.h
 * author   : sundayman66@gmail.com
 * date     : 2012-8-14
 * brief    : 
 */

#ifndef ECHO_CONTROLLER_H_
#define ECHO_CONTROLLER_H_

#include <tchar.h>

#include "../../third_party/tinyxml/tinyxml.h"

#include "def.h"
#include "../order/ack_sender.h"

namespace Net {

class EchoController : public Command{

public:

	EchoController(const char *name): Command(name) {

	}

	virtual void Excute(const DiplomatPtr &diplomat, TiXmlElement *params,
			const char *version, const char *cmd_id) {

	    TiXmlElement* echoEle = params->FirstChildElement("echo");
	    if ( echoEle )
	    {
	        const char* txt = echoEle->GetText();
	        if ( txt )
	        {
	        	AckSender ack_sender(_name, true, txt);
	        	ack_sender.Excute(diplomat, cmd_id);
	            return;
	        }
	    }

		AckSender ack_sender(_name, false, "invalid xml format");
		ack_sender.Excute(diplomat, cmd_id);
	    AppLog(APP_LOG_ERR, _T("net: recv invalid echo method"));
		return;
	}
protected:

};

};

#endif /* ECHO_CONTROLLER_H_ */


