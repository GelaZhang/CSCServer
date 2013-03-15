/* 
 * filename : sys_time_sender.h
 * author   : sundayman66@gmail.com
 * date     : 2012-8-16
 * brief    : 
 */

#ifndef SYS_TIME_SENDER_H_
#define SYS_TIME_SENDER_H_


#include "order.h"

#ifdef WIN32
#include <time.h>

#define snprintf _snprintf
#endif

namespace Net {

class SysTimeSender : public Order {

public:
	SysTimeSender() : Order("GetSystemTime"){}

protected:
	virtual void PrepareCommand(const DiplomatPtr &diplomat, const char* command_id) {

	    AppLog(APP_LOG_INFO, _T("net: send system time"));

        time_t t = time(NULL);
        tm utc_t;
        //gmtime_r(&t, &utc_t); //将utc时间改为本地时间
#ifndef WIN32
        localtime_r(&t, &utc_t);
#else
        localtime_s(&utc_t, &t);
#endif
        char time[32 + 1];
        snprintf(time, 32, "%d-%d-%d %d:%d:%d",utc_t.tm_year + 1900,
            utc_t.tm_mon + 1, utc_t.tm_mday, utc_t.tm_hour, utc_t.tm_min,
            utc_t.tm_sec);
        TiXmlElement* timeEle = new TiXmlElement("time");
        timeEle->LinkEndChild( new TiXmlText(time));
        _order_params->LinkEndChild(timeEle);
	}

};

};
#endif /* SYS_TIME_SENDER_H_ */


