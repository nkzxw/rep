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

#define Log0(priority) log_msg(priority, "%s:%d:%s()", __FILE__, __LINE__, __FUNCTION__)
#define Log1(priority, fmt) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__)
#define Log2(priority, fmt, data) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data)
#define Log3(priority, fmt, data1, data2) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2)
#define Log4(priority, fmt, data1, data2, data3) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3)
#define Log5(priority, fmt, data1, data2, data3, data4) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3, data4)
#define Log6(priority, fmt, data1, data2, data3, data4, data5) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3, data4, data5)
#define Log7(priority, fmt, data1, data2, data3, data4, data5, data6) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3, data4, data5, data6)
#define Log8(priority, fmt, data1, data2, data3, data4, data5, data6, data7) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3, data4, data5, data6, data7)
#define Log9(priority, fmt, data1, data2, data3, data4, data5, data6, data7, data8) log_msg(priority, "%s:%d:%s() " fmt, __FILE__, __LINE__, __FUNCTION__, data1, data2, data3, data4, data5, data6, data7, data8)


LOG_API void log_msg(const int priority, const char *fmt, ...);
//	__attribute__((format(printf, 2, 3)));

LOG_API void debug_msg(const int priority, const char *fmt, ...);

LOG_API	int set_fd(FILE *fd);
LOG_API char getDebugLogType();
LOG_API void DebugLogSetLogType(const int);
LOG_API char getDebugLogLevel();
LOG_API void DebugLogSetLevel(const int level);

#ifdef __cplusplus
}
#endif

#endif/* __debuglog_h__ */

