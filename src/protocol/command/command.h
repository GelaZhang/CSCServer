/* 
 * filename : command.h
 * author   : zhangjiaqin@zlg.cn
 * date     : 2012-7-11
 * brief    : 
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string.h>

#include "util/ref_counter.h"

#include "diplomat.h"

class TiXmlElement;

namespace CSCServer {

class Command : public Utility::RefCounter {

public:
	Command(const char *name) {
		if ( name ) {
			_name = new char[strlen(name) + 1];
			strcpy(_name, name);
		} else {
			_name = new char[16];
			strcpy(_name, "no-name");
		}
	}
	virtual ~Command() {
		if ( _name ) {
			delete []_name;
			_name = NULL;
		}
	}
	const char* Name() {
		return _name;
	}

	virtual void Excute(const DiplomatPtr &diplomat, TiXmlElement *params,
			const char *version, const char *cmd_id) = 0;

protected:
	char *_name;
};
typedef Utility::SmartPtr<Command> CommandPtr;

};
#endif /* COMMAND_H_ */


