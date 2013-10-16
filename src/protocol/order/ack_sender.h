/* 
 * filename : ack_sender.h
 * author   : sundayman66@gmail.com
 * date     : 2012-8-15
 * brief    : 
 */

#ifndef ACK_SENDER_H_
#define ACK_SENDER_H_

#include "order.h"

namespace CSCServer {


class AckSender : public Order {

public:
	AckSender(const char* cmd_name,
			bool result, const char* reason) : Order("Ack"){
		_result = result;
		if ( cmd_name ) {
			_cmd_name = (char*)malloc(strlen(cmd_name) + 1);
			strcpy(_cmd_name, cmd_name);

		} else {
			_cmd_name = (char*)malloc(8 + 1);
			strcpy(_cmd_name, "unknow");
		}
		if ( reason ) {
			_reason = (char*)malloc(strlen(reason) + 1);
			strcpy(_reason, reason);

		} else {
			_reason = NULL;
		}
	}
	~AckSender() {
		if ( _cmd_name )
			free(_cmd_name);
		if ( _reason )
			free(_reason);
	}

protected:
	virtual void PrepareCommand(const DiplomatPtr &diplomat, const char* command_id) {

	    TiXmlElement* responseEle = new TiXmlElement("response");
	    TiXmlText* responseContent = new TiXmlText(_cmd_name);
	    responseEle->LinkEndChild(responseContent);
	    _order_params->LinkEndChild(responseEle);
	    TiXmlElement* resultEle = new TiXmlElement("result");
	    if ( _result )
	    {
	        resultEle->LinkEndChild(new TiXmlText("true"));
	    }
	    else if ( _reason )
	    {
	        resultEle->LinkEndChild(new TiXmlText("false"));
	        TiXmlElement* exception = new TiXmlElement("exception");
	        exception->LinkEndChild(new TiXmlText(_reason));
	        _order_params->LinkEndChild(exception);

	    }
	    else
	    {
	        resultEle->LinkEndChild(new TiXmlText("false"));
	    }
	    _order_params->LinkEndChild(resultEle);
	}

protected:
	bool _result;
	char *_cmd_name;
	char *_reason;
};

};
#endif /* ACK_SENDER_H_ */


