/*
 * fdfs2qq_define.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_FDFS2QQ_DEFINE_H_
#define SRC_FDFS2QQ_FDFS2QQ_DEFINE_H_
#include "common_define.h"
namespace fdfs2qq{


const std::string LOGPREFIX="/data/log/fdfs2qq";


int GetCpuCoreCount(){
	return (int)sysconf(_SC_NPROCESSORS_CONF);
}


}




#endif /* SRC_FDFS2QQ_FDFS2QQ_DEFINE_H_ */
