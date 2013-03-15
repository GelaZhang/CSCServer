/*
 * \filename: def.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#ifndef DEF_H_
#define DEF_H_

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifdef WIN32
#define bzero(s, n) memset(s, 0, n)
#endif

enum ErrorCode {
	kOK = 0,  // everything is good
	kError,   //  unknow error
	kLibEventErr  //something wrong with libevent
};

#define APP_LOG_DEBUG 0
#define APP_LOG_INFO   1
#define APP_LOG_WARN  2
#define APP_LOG_ERR   3

#include "log4cxx/logger.h"
#include "log4cxx/PropertyConfigurator.h"

void __cdecl AppLog(int level, const wchar_t *format, ...);

void __cdecl AppLog(int level, const char *format, ...);

#endif /* DEF_H_ */
