/*
 * logger.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_LOGGER_H_
#define SRC_FDFS2QQ_LOGGER_H_
#include <cstdarg>
#include "common_define.h"
#include "ganji/util/log/thread_fast_log.hpp"
namespace FastLog = ganji::util::log::ThreadFastLog;

namespace fdfs2qq {

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif
class Logger {
public:
	static void debug(const std::string);
	static void info(const std::string);
	static void error(const std::string);

	static void debug(const char *format, ...);
	static void info(const char *format, ...);
	static void error(const char *format, ...);
};

}
inline void logEmerg(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logDebug(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logInfo(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logNotice(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logWarning(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logError(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logAlert(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}
inline void logCrit(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
}

#endif /* SRC_FDFS2QQ_LOGGER_H_ */
