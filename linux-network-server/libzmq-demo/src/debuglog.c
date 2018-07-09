#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>

#include "debuglog.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef COUNT_OF
#define COUNT_OF(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

/**
 * Max string size dumping a maxmium of 2 lines of 80 characters
 */
#define DEBUG_BUF_SIZE 4096
static char LogMsgType = DEBUGLOG_SYSLOG_DEBUG;
/** default level */
static char LogLevel = APP_LOG_ERROR;

static signed char LogDoColor = 0;	/**< no color by default */

static FILE *filefd=0;
static const char* desc[] = {
	"NOTICE",		// critical conditions
	"INFO",			// informational
	"DEBUG",			// debug-level messages
	"WARNING",		// warning conditions
	"ERROR",       	// error conditions
	"CRITICAL",		// critical messages
};


int set_fd(FILE *fd)
{
	if(fd != NULL) {
		filefd = fd;
		return 0;
	}
	
	return -1;
}

static void log_line(const int priority, const char *DebugBuffer);

void log_msg(const int priority, const char *fmt, const char* FileName, int LineNumber, ...)
{
	char DebugBuffer[DEBUG_BUF_SIZE]="\0";
	va_list argptr;

	if ((priority < LogLevel) /* log priority lower than threshold? */
		|| (DEBUGLOG_NO_DEBUG == LogMsgType))
		return;
	time_t timep;
	time(&timep);
	char* t = ctime(&timep);
	t[(int)strlen(t)-1] = '\0';
	va_start(argptr, LineNumber);
	fprintf(filefd,"%s %s %s:%u ", t, desc[priority], FileName, LineNumber);
	vfprintf(filefd, fmt, argptr);
	//sprintf(filefd,"%s %s:%u:%s ", ctime(&timep), FileName, LineNumber, FuncName);
	//vsnprintf(DebugBuffer + ret, sizeof DebugBuffer, fmt, argptr);
	va_end(argptr);
	fprintf(filefd, "\n");  
	fflush(filefd);
	//log_line(priority, DebugBuffer);
} /* log_msg */

static void log_line(const int priority, const char *DebugBuffer)
{
	char dateTime[32]="\0";
	time_t t = time(0);
	sprintf(dateTime,"%s",asctime(localtime(&t)));
	memset(dateTime+strlen(dateTime)-1,0,32-strlen(dateTime)-1);
	
	if (DEBUGLOG_SYSLOG_DEBUG == LogMsgType){
	#ifdef __linux__
		char tmp=getDebugLogLevel();
		switch (tmp){
			case APP_LOG_NOTICE:
				syslog(APP_LOG_NOTICE ,"%s", DebugBuffer);
				break;
			case APP_LOG_INFO:
				syslog(APP_LOG_INFO ,"%s", DebugBuffer);
				break;
			case APP_LOG_DEBUG:
				syslog(APP_LOG_DEBUG ,"%s", DebugBuffer);
				break;
			case APP_LOG_WARNING:
				syslog(APP_LOG_WARNING ,"%s", DebugBuffer);
				break;
			case APP_LOG_ERROR:
				syslog(APP_LOG_ERROR ,"%s", DebugBuffer);
				break;
			case APP_LOG_CRITICAL:
				syslog(APP_LOG_CRITICAL ,"%s", DebugBuffer);
				break;
			default:
				syslog(APP_LOG_CRITICAL ,"default");
				break;
		}
	#endif
	}
	else if(DEBUGLOG_STDOUT_FILE == LogMsgType){
		fprintf(filefd, "%s\n", DebugBuffer);
	}
	else if(DEBUGLOG_STDOUT_DEBUG == LogMsgType || DEBUGLOG_STDOUT_COLOR_DEBUG == LogMsgType){
		if (LogDoColor){
			const char *color_pfx = "", *color_sfx = "\33[0m";
			const char *time_pfx = "\33[39m", *time_sfx = color_sfx;

			switch (priority){
				case APP_LOG_CRITICAL:
//					color_pfx = "\33[01,31m"; /* Magenta */
					color_pfx = "\33[31m"; /* Magenta */
					break;
				case APP_LOG_ERROR:
					color_pfx = "\33[32m"; /* Magenta */
					break;
				case APP_LOG_WARNING:
					color_pfx = "\33[33m"; /* bright + Red */
					break;
				case APP_LOG_DEBUG:
					color_pfx = "\33[34m"; /* Magenta */
					break;
				case APP_LOG_INFO:
					color_pfx = "\33[35m"; /* Blue */
					break;
				case APP_LOG_NOTICE:
					color_pfx = "\33[36m"; /* normal (black) */
					break;
				default:
					color_pfx = ""; /* normal (black) */
					color_sfx = "";
					break;
			}
			printf("%s %s %s %s %s\n", time_pfx, time_sfx, color_pfx, DebugBuffer, color_sfx);
		}
		else{
				printf("%s\n", DebugBuffer);
		}
		fflush(stdout);
	}
	else {
		printf("LogMsgType£º set error\n");
	}
} /* log_line */

void DebugLogSetLogType(const int dbgtype)
{
	switch (dbgtype){
		case DEBUGLOG_NO_DEBUG:
		case DEBUGLOG_SYSLOG_DEBUG:
		case DEBUGLOG_STDOUT_DEBUG:
		case DEBUGLOG_STDOUT_COLOR_DEBUG:
		case DEBUGLOG_STDOUT_FILE:
			LogMsgType = dbgtype;
			break;
		default:
			bw_log(APP_LOG_ERROR, "unknown log type (%d), using stdout",dbgtype);
			LogMsgType = DEBUGLOG_STDOUT_DEBUG;
			break;
	}

	/* log to stdout and stdout is a tty? */
	if ((DEBUGLOG_STDOUT_DEBUG == LogMsgType) || (DEBUGLOG_STDOUT_COLOR_DEBUG == LogMsgType) && isatty(fileno(stdout))) {
//		if (DEBUGLOG_STDOUT_COLOR_DEBUG == LogMsgType){
		char *term;

		term = getenv("TERM");
		if (term){
			const char *terms[] = { "linux", "xterm", "xterm-color", "Eterm", "rxvt", 
				"rxvt-unicode", "xterm-256color" };
			unsigned int i;

			/* for each known color terminal */
			for (i = 0; i < COUNT_OF(terms); i++){
				/* we found a supported term? */
				if (0 == strcmp(terms[i], term)){
					LogDoColor = 1;
					break;
				}
			}
		}
	}
}

void DebugLogSetLevel(const int level)
{
	LogLevel = level;
	switch (level){
		case APP_LOG_NOTICE:
//			bw_log(APP_LOG_NOTICE, "debug level=notice");
			break;
		case APP_LOG_INFO:
//			bw_log(APP_LOG_INFO, "debug level=info");
			break;
		case APP_LOG_DEBUG:
//			bw_log(APP_LOG_DEBUG, "debug level=debug");
			break;
		case APP_LOG_WARNING:
//			bw_log(APP_LOG_WARNING, "debug level=warning");
			break;
		case APP_LOG_ERROR:
//			bw_log(APP_LOG_ERROR, "debug level=error");
			break;
		case APP_LOG_CRITICAL:
//			bw_log(APP_LOG_CRITICAL, "debug level=critical");
			break;
		default:
			LogLevel = APP_LOG_ERROR;
//			bw_log(APP_LOG_ERROR, "unknown level (%d), using level=info",level);
			break;
	}
}

char getDebugLogLevel()
{
    return LogLevel;
}

char getDebugLogType()
{
    return LogMsgType;
}
/*
 * old function supported for backward object code compatibility
 * defined only for pcscd
 */
void debug_msg(const int priority, const char *fmt, ...)
{
	char DebugBuffer[DEBUG_BUF_SIZE]="\0";
	va_list argptr;
		
	if ((priority < LogLevel) /* log priority lower than threshold? */
		|| (DEBUGLOG_NO_DEBUG == LogMsgType))
		return;

	va_start(argptr, fmt);
	vsnprintf(DebugBuffer, sizeof DebugBuffer, fmt, argptr);
	va_end(argptr);

	if (DEBUGLOG_SYSLOG_DEBUG == LogMsgType){
	    syslog(priority, "%s", DebugBuffer);
	}
	else if(DEBUGLOG_STDOUT_FILE == LogMsgType && filefd != NULL){
	    fprintf(filefd, "%s\n", DebugBuffer);
	}
	else
	    puts(DebugBuffer);
} /* debug_msg */

#ifdef __cplusplus
}
#endif

