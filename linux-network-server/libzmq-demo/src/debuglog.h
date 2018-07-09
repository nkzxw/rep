#ifndef __debuglog_h__
#define __debuglog_h__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LOG_API
#define LOG_API
#endif

enum {
	DEBUGLOG_NO_DEBUG = 0, //不打印log
	DEBUGLOG_STDOUT_FILE, //输出到指定文件
	DEBUGLOG_SYSLOG_DEBUG,//log输出到系统日志
	DEBUGLOG_STDOUT_DEBUG,//log输出到终端
	DEBUGLOG_STDOUT_COLOR_DEBUG,//log输出到终端，且带颜色
};

enum {
	APP_LOG_NOTICE = 0,		// critical conditions
	APP_LOG_INFO,			// informational
	APP_LOG_DEBUG,			// debug-level messages
	APP_LOG_WARNING,		// warning conditions
	APP_LOG_ERROR,       	// error conditions
	APP_LOG_CRITICAL,		// critical messages
};

/* You can't do #ifndef __FUNCTION__ */
#if !defined(__GNUC__) && !defined(__IBMC__)
#define __FUNCTION__ ""
#endif

#ifndef __GNUC__
#define __attribute__(x) /*nothing*/
#endif

//#define bw_log(priority, fmt, ...) log_msg(priority, fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define bw_log(priority, fmt, args ...) log_msg(priority, (fmt), __FILE__, __LINE__, ##args)


//LOG_API void log_msg(const int priority, const char *fmt, ...);
//	__attribute__((format(printf, 2, 3)));

//LOG_API void debug_msg(const int priority, const char *fmt, ...);

void log_msg(const int priority, const char *fmt, const char* SavedFileName, int SavedLineNumber, ...);
LOG_API	int set_fd(FILE *fd);
LOG_API char getDebugLogType();
LOG_API void DebugLogSetLogType(const int);
LOG_API char getDebugLogLevel();
LOG_API void DebugLogSetLevel(const int level);

#ifdef __cplusplus
}
#endif

#endif/* __debuglog_h__ */

