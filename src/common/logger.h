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

class Logger {
public:
	static void  debug(const std::string);
	static void info(const std::string );
	static void error(const std::string );
};


}
#endif /* SRC_FDFS2QQ_LOGGER_H_ */
