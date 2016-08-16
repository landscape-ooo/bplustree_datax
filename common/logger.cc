/*
 * logger.cc
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#include "logger.h"
namespace fdfs2qq {

void Logger::info(const std::string msg) {
	std::cout << "info:.." << msg << std::endl;
}
void Logger::error(const std::string msg) {
	std::cout << "eroor:.." << msg << std::endl;
}
void Logger::debug(const std::string msg) {
	std::cout << "debug:.." << msg << std::endl;
}


}
