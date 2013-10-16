/*
 * \filename: env_master.cpp
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */
#if _WIN32
#include <tchar.h>
#endif
#include <stdio.h>
#include "env_master.h"
#include "event.h"
#include "def.h"
using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace CSCServer;
void log_interface(int severity, const char *msg) {
	static log4cxx::LoggerPtr logger(Logger::getLogger("Libevent"));
	printf("%s", msg);
	switch(severity) {
	case EVENT_LOG_DEBUG:
		logger->debug(msg);
		break;
	case EVENT_LOG_MSG:
		logger->info(msg);
		break;
	case EVENT_LOG_WARN:
		logger->warn(msg);
		break;
	case EVENT_LOG_ERR:
		logger->error(msg);
		break;
	default:
		logger->debug(msg);
	}
}

void EnvMaster::SetLogInterface() {

	event_set_log_callback(log_interface);
}
#if _WIN32
void __cdecl AppLog(int level, const wchar_t *format, ...)
{
	static log4cxx::LoggerPtr logger(Logger::getLogger(L"App"));
	va_list args;
	va_start(args, format);

	int nBuf;
	wchar_t szBuffer[4096]= L"";


	nBuf = _vsnwprintf(szBuffer, _countof(szBuffer), format, args);

	va_end(args);
	switch(level) {
	case APP_LOG_DEBUG:
		logger->debug(szBuffer);
		break;
	case APP_LOG_INFO:
		logger->info(szBuffer);
		break;
	case APP_LOG_WARN:
		logger->warn(szBuffer);
		break;
	case APP_LOG_ERR:
		logger->error(szBuffer);
		break;
	default:
		logger->debug(szBuffer);
	}

}
#endif
void __cdecl AppLog(int level, const char *format, ...)
{
	static log4cxx::LoggerPtr logger(Logger::getLogger("App"));
	va_list args;
	va_start(args, format);

	int nBuf;
	char szBuffer[4096]= "";
#if _WIN32
	nBuf = _vsnprintf(szBuffer, _countof(szBuffer), format, args);
#else
	nBuf = vsnprintf(szBuffer, _countof(szBuffer), format, args);
#endif
	va_end(args);
	switch(level) {
	case APP_LOG_DEBUG:
		logger->debug(szBuffer);
		break;
	case APP_LOG_INFO:
		logger->info(szBuffer);
		break;
	case APP_LOG_WARN:
		logger->warn(szBuffer);
		break;
	case APP_LOG_ERR:
		logger->error(szBuffer);
		break;
	default:
		logger->debug(szBuffer);

	}

}
