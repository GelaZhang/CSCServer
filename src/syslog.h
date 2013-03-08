#ifndef _SYS_LOG_H
#define _SYS_LOG_H

#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#define LOG_CRIT        2                                               /* critical conditions          */
#define LOG_ERR         3                                               /* error conditions             */
#define LOG_WARNING     4                                               /* warning conditions           */
#define LOG_NOTICE      5                                               /* normal but significant       */
/* condition                    */
#define LOG_INFO        6                                               /* informational                */
#define LOG_DEBUG       7    

static void     syslog(int priority, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    char szBuffer[4096]="";

    vsnprintf(szBuffer, 4096, message, args);

    va_end(args);
#ifdef WIN32
    OutputDebugStringA(szBuffer);
#endif


    printf("%s", szBuffer);

}
#endif //!_SYS_LOG_H
