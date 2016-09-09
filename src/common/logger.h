/*
 * logger.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_LOGGER_H_
#define SRC_FDFS2QQ_LOGGER_H_
#include "common_define.h"
namespace fdfs2qq{

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif
class Logger {
public:
	static void  debug(const std::string);
	static void info(const std::string );
	static void error(const std::string );

	static void debug(const char *format, ...);
	static void info(const char *format, ...);
	static void error(const char *format, ...);
};


}
#endif /* SRC_FDFS2QQ_LOGGER_H_ */
