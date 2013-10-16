/*
 * command_distributor.cpp
 *
 *  Created on: 2012-7-11
 *      Author: sundayman66@gmail.com
 */
#include "default_protocol.h"
#if _WIN32
#include <tchar.h>
#endif
#include <sstream>

#include "tinyxml.h"

#include "def.h"
#include "protocol_def.h"

#include "order/sys_time_sender.h"

using namespace CSCServer;
using namespace std;

const char ProtocolHeader::kProtocolStart[] = "protocol:";
const char ProtocolHeader::kProtocolVersion[] = "version:";
const char ProtocolHeader::kProtocolPubkey[] = "pub-key:";
const char ProtocolHeader::kProtocolSize[] = "size:";
const char ProtocolHeader::kProtocolCmdId[] = "cmd-id:";
const char ProtocolHeader::kProtocolSeparator[] = "\n";
const char ProtocolHeader::kProtocolSeparator2[] = "\r\n";

const int DefaultProtocol::kMaxLengthOfHeader = 1024;
const int DefaultProtocol::kMaxLengthOfMethod = 64 * 1024;
const int DefaultProtocol::kMaxBuffer = 64 * 1024;
DefaultProtocol::CmdMap DefaultProtocol::s_cmds;

ProtocolHeader::ProtocolHeader() {
	strcpy(protocol, PROTOCOL_NAME);
	strcpy(version, PROTOCOL_VERSION);
	strcpy(pubkey, PROTOCOL_NOT_SET);
	strcpy(commandId, PROTOCOL_NOT_SET);
	size = 0;
}

string ProtocolHeader::BuildHeader(int size, const char *ack_id) {

	if (  !ack_id )
		return "";
	ostringstream str;
	str<<kProtocolStart<<protocol<<"\n"<<kProtocolVersion<<version<<"\n"
			<<kProtocolPubkey<<pubkey<<"\n"
			<<kProtocolSize<<size<<"\n"<<kProtocolCmdId<<ack_id<<"\n";
	return str.str();
}


int DefaultProtocol::RecvSomething() {

	if ( !_diplomat )
		return 0;

	size_t length = _diplomat->GetMassageLength();
	if ( length <= 0 ) {

		AppLog(APP_LOG_ERR,
			_T("net: didn't recv anything\n") );
		return 0;
	}
	assert(_memo);
	_memo->PrepareCapacity(length);
	_memo->_buffer_offset += _diplomat->ReadMassage(_memo->_buffer + _memo->_buffer_offset,
		_memo->_buffer_length - _memo->_buffer_offset, length);

	int cmd_cnt = 0;
	int cursor = 0;
	char *buffer = reinterpret_cast<char*>(_memo->_buffer);
	length = _memo->_buffer_offset;
	for (;cursor < length;) {

		if ( !_memo->_header_got ) {

			int ret = GetProtocolHeader( buffer, cursor, length);
			if ( -1 == ret )
				break;
			else if ( 1 == ret )
				continue;
		}

		if ( _memo->_header_got ) {

			ProtocolHeader &header =_memo->_header;
			if ( header.size <= length - cursor ) {
				char tmp = buffer[cursor + header.size];
				buffer[cursor + header.size] = '\0';
				ParseCommand(buffer + cursor, header.version);
				buffer[cursor + header.size] = tmp;
				cursor += header.size;

				_memo->_header_got = false;
				cmd_cnt ++;
				continue;

			} else if ( header.size > kMaxLengthOfMethod ) {
				_memo->_header_got = false;
				cursor = length;
				break;
			} else {

#if ENABLE_DEBUG_LOG
				AppLog(APP_LOG_INFO, _T("net: no recv a complete pagkage; size:%d recv:%d "),
						header.size, length - cursor);
#endif
				break;
			}

		}

	}
	_memo->Drain(cursor);
	return cmd_cnt;

}


bool DefaultProtocol::RegisterCommand(const CommandPtr &command) {

	s_cmds[command->Name()] = command;
	return true;
}


int DefaultProtocol::ParseProtocolHeader(const char* data) {

	ProtocolHeader &header = _memo->_header;
	const char* tmp = GetProtocolKeyword(data, header.kProtocolCmdId,
        header.commandId, sizeof(header.commandId) - 1 );
    if ( NULL == tmp) {

        return 0;
    }

    int ret = tmp - data + 1;
    _memo->_header_length = ret;

    tmp = GetProtocolKeyword(data, header.kProtocolStart,
            header.protocol, sizeof(header.protocol) -1 );
    if (  NULL == tmp ) {

        AppLog(APP_LOG_ERR, _T("net: protocol header do no include protocol keyword\n") );
        return -1;
    }

    tmp = GetProtocolKeyword(tmp, header.kProtocolVersion,
            header.version, sizeof(header.version) -1 );
    if (  NULL == tmp ) {

        AppLog(APP_LOG_ERR, _T("net: protocol header do no include %s keyword: %s\n"),
        		header.kProtocolVersion, data);
        return -1;
    }

    tmp = GetProtocolKeyword(tmp, header.kProtocolPubkey,
            header.pubkey, sizeof(header.pubkey) - 1 );
    if (  NULL == tmp ) {

        AppLog(APP_LOG_ERR, _T("net: protocol header do no include pub-key keyword\n"));
        return -1;
    }

    char size[32];
    memset(size, 0, 32);
    tmp = GetProtocolKeyword(tmp, header.kProtocolSize,
            size, sizeof(size) - 1 );
    if (  NULL == tmp ) {

        AppLog(APP_LOG_ERR, _T("net: protocol header do no include size keyword\n"));
        return -1;
    }
    header.size = atoi(size);

    const char *foo = GetProtocolKeyword(tmp, header.kProtocolSize,
            size, sizeof(size) - 1 );
    if ( foo && foo - data + 1 < ret )
    {
        AppLog(APP_LOG_ERR, _T("net: there are two 'size' keyword between cmd id\n"));
        return -1;
    }

    _memo->_header_got = true;
    return ret;

}

int DefaultProtocol::GetProtocolHeader(char *buffer, int &cursor, int length) {

	int ret = ParseProtocolHeader(buffer + cursor);
	if ( 0 == ret ) {

		for ( int i = 0;length - cursor > kMaxLengthOfHeader && 0 == ret; i ++) {

			if ( i == 0)
				AppLog(APP_LOG_INFO,
				_T("net: buf is more than max haeder len but do not recv header\n"));
			cursor += strlen(_memo->_header.kProtocolStart); //丢弃第一个关键字长度

			ret = ParseProtocolHeader(buffer + cursor);
		}

		if ( 0 == ret ) {
			return -1;
		}

	}

	if ( -1 == ret ) {

		cursor += _memo->_header_length;
		AppLog(APP_LOG_INFO, _T("net: recv illegal package\n"));
		return 1;

	} else {

		cursor += ret;
		return 0;
	}

	AppLog(APP_LOG_ERR, _T("net: out of control"));
	return 0;
}


const char *DefaultProtocol::GetProtocolKeyword(const char *data, const char *keyword_start,
        char *keyword, int keyword_len) {

    const char* protocol_start = strstr(data, keyword_start);
    if ( NULL == protocol_start ) {
        return NULL;
    }

    const char *separator = strstr(protocol_start + strlen(keyword_start),
    		ProtocolHeader::kProtocolSeparator);
    if ( NULL == separator ) {

    	separator = strstr(protocol_start + strlen(keyword_start),
    	    		ProtocolHeader::kProtocolSeparator2);
    	if ( !separator )
    		return NULL;
    }

    int maxTmp = separator - protocol_start - strlen(keyword_start) > keyword_len - 1 ?
            keyword_len - 1: separator - protocol_start - strlen(keyword_start);
    memcpy(keyword, protocol_start + strlen(keyword_start), maxTmp);
    keyword[maxTmp] = '\0';

    return separator;
}


int DefaultProtocol::ParseCommand(char *content, char *version) {

	TiXmlDocument commands;
	commands.Parse(content);
	TiXmlElement *root_ele = commands.RootElement();
	if (root_ele == NULL) {
		AppLog(APP_LOG_ERR, _T("net: get root element error"));
		return 0;
	}

	int ret = 0;
	//考虑到一个xml可能包括多个method
	for (TiXmlElement *method_ele = root_ele->FirstChildElement("method");
			method_ele; method_ele = method_ele->NextSiblingElement()) {

		TiXmlElement *name_ele = method_ele->FirstChildElement("name");
		if (NULL == name_ele) {
			AppLog(APP_LOG_ERR, _T("net: can not find method name node"));
			continue;
		}

		const char* cmd_id = name_ele->Attribute("id");
		const char* name = name_ele->GetText();
		if	( !name || !cmd_id ) {
			AppLog(APP_LOG_ERR, _T("net: can not find method name value"));
			continue;
		}

		TiXmlElement *params_ele = method_ele->FirstChildElement("params");
		if ( NULL == params_ele ) {
			AppLog(APP_LOG_ERR, _T("net: method:%s parameter is invalid"), name);
			continue;
		}

		CmdMap::iterator cmd = s_cmds.find(name);
		if ( cmd != s_cmds.end() ) {
			cmd->second->Excute(_diplomat, params_ele, version, cmd_id);

		} else {
			AppLog(APP_LOG_DEBUG, _T("net: method no handle :%s"), name);

		}

		ret ++;
	}

	return ret;

}

DefaultProtocol::DefaultProtocol(const DiplomatPtr &diplomat) : Protocol(diplomat){
	_memo = NULL;
}

int DefaultProtocol::GarrisonDiplomat() {

	_memo = new DiplomatMemo();

	return 0;
}


int DefaultProtocol::WithdrawDiplomat() {

	_memo->Drain(-1); //清空接收缓冲
	return 0;
}

