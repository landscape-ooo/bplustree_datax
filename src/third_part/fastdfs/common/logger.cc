/*
 * logger.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "logger.h"
namespace fdfs2qq {

void Logger::info(const std::string msg) {
	FastLog::WriteLog(FastLog::kLogNotice, msg.c_str());
//	std::cout << "info:.." << msg << std::endl;
}
void Logger::info(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogNotice, msg.c_str());
//	std::cout << "info:.." << msg << std::endl;
}
void Logger::error(const std::string msg) {
	FastLog::WriteLog(FastLog::kLogFatal, msg.c_str());
//	std::cout << "eroor:.." << msg << std::endl;
}
void Logger::error(const char *format, ...) {
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
//	std::cout << "eroor:.." << msg << std::endl;
}

void Logger::debug(const std::string msg) {
	FastLog::WriteLog(FastLog::kLogDebug, msg.c_str());
//	std::cout << "debug:.." << msg << std::endl;
}
void Logger::debug(const char *format, ...) {
	char text[LINE_MAX];
	int len;
	{
		va_list ap;
		va_start(ap, format);
		len = vsnprintf(text, sizeof(text), format, ap);
		va_end(ap);
	}
	std::string msg(text, len);
	FastLog::WriteLog(FastLog::kLogDebug, msg.c_str());
//	std::cout << "debug:.." << msg << std::endl;
}

}



