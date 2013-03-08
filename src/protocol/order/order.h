/* 
 * filename : order.h
 * author   : sundayman66@gmail.com
 * date     : 2012-8-3
 * brief    : 
 */

#ifndef ORDER_H_
#define ORDER_H_

#include "tinyxml.h"

#include "diplomat.h"
#include "../command_distributor.h"

namespace Net {

class Order {

public:
	Order(const char *name);

	virtual ~Order(){}

	virtual bool Excute(const DiplomatPtr &diplomat, const char* command_id);

	static void InitOrder();

protected:
	virtual void PrepareCommand(const DiplomatPtr &diplomat, const char* command_id){}

protected:
	void InitOrder(const char *name);

	void AddParamEle(const char* attribute, const char* content);

	bool SendAux(const DiplomatPtr &diplomat, const char *command_id);
protected:
	static int s_id;
	static ProtocolHeader s_header;
	TiXmlDocument _order;
	TiXmlElement* _method;
	TiXmlElement *_order_params;

};

};
#endif /* ORDER_H_ */


