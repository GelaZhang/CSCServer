/*
 * Order.cpp
 *
 *  Created on: 2012-8-3
 *      Author: sundayman66@gmail.com
 */

#include "order.h"

#ifndef WIN32
#include <strings.h>
#endif

#include <string>

#include "def.h"
#include "../protocol_def.h"

using namespace CSCServer;
using namespace std;

int Order::s_id = 0;
ProtocolHeader Order::s_header;
Order::Order(const char *name) {

	InitOrder(name);
}


bool Order::Excute(const DiplomatPtr &diplomat, const char* command_id) {

	if ( !diplomat ) {
		return false;
	}
	PrepareCommand(diplomat, command_id);
    if ( SendAux(diplomat, command_id) ) {
    	return true;

    } else {
    	AppLog(APP_LOG_DEBUG, "net: send order  fail");
    	return false;
    }
	return true;

}


void Order::InitOrder() {
}


void Order::InitOrder(const char *name) {

    TiXmlDeclaration* dec = new TiXmlDeclaration("1.0", "utf-8", "");
    _order.LinkEndChild(dec);
    //根节点methods
    TiXmlElement* rootEle = new TiXmlElement("methods");
    _order.LinkEndChild(rootEle);
    //method节点
    _method = new TiXmlElement("method");
    rootEle->LinkEndChild(_method);
    //name节点
    TiXmlElement* nameEle = new TiXmlElement("name");
    _method->LinkEndChild(nameEle);
    TiXmlText* nameContent = NULL;
    if ( name )
    	nameContent = new TiXmlText(name);
    else
    	nameContent = new TiXmlText("unknown");
    nameEle->LinkEndChild(nameContent);
    //params节点
    _order_params = new TiXmlElement("params");
    _method->LinkEndChild(_order_params);

    _method->SetAttribute("id", s_id ++);
}


void Order::AddParamEle(const char* attribute, const char* content)
{
	TiXmlElement* paramEle = new TiXmlElement("param");
	_order_params->LinkEndChild(paramEle);
	paramEle->SetAttribute("name", attribute);
	TiXmlText* paramContent = new TiXmlText(content);
	paramEle->LinkEndChild(paramContent);
}


bool Order::SendAux(const DiplomatPtr &diplomat, const char *command_id) {

	TiXmlPrinter printer;
	_order.Accept(&printer);

	int size = strlen(printer.CStr());
	string header;
	if ( command_id )
		header = s_header.BuildHeader(size, command_id);
	else
		header = s_header.BuildHeader(size, PROTOCOL_NOT_SET);

	header += printer.CStr();

	int ret = diplomat->Speek(reinterpret_cast<const BYTE*>(header.c_str()), header.size());
	if(ret < 0)
		return false;
	return true;
}
