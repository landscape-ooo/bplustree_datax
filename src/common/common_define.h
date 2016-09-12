/*
 * common_define.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_COMMON_DEFINE_H_
#define SRC_FDFS2QQ_COMMON_DEFINE_H_
#include <queue>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <glob.h>
#include <cstdio>
#include <cerrno>

#include <openssl/md5.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cstring>

// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>
#include <cctype>
#include <string>
#include <functional>
#include <algorithm>


#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/unistd.h>

#include "common/INIReader.h"
using namespace std;
namespace fdfs2qq{
	//k\tv{record}k2\tv2
	const char BOX_KV_SEPERATOR='\x09';// \t
	const char BOX_RECORD_SEPERATOR='\x01';//item split
	const char BOX_FIELD_SEPERATOR='\x02';//file type split
	const char BOX_MSG_SEPERATOR='\x03';//msg type split

	const int MAX_BUFFER_SIZE = 1024;

	const int GetCpuCoreCount=(int)sysconf(_SC_NPROCESSORS_CONF);

	const static INIReader* _IReaderHandle=NULL;
	const static INIReader* GetIniReader(){
		if(_IReaderHandle==0){
			_IReaderHandle=new  INIReader("/root/cpp_local/fdfs_bpt/src/unittest_bin/test.ini");
		}
		return _IReaderHandle;
	}
	inline const string TRACKER_IP(){
		return GetIniReader()->Get("tracker","host","");
	}
	inline const int TRACKER_PORT(){
		return GetIniReader()->GetInteger("tracker","port",0);
	}
	inline const int MAX_GRP_ID(){
		return GetIniReader()->GetInteger("storage","group_count",0);
	}
	inline const std::string LOGPREFIX(){
		return GetIniReader()->Get("logger","prefix","");
	}
	inline const std::string BINLOG_LOGPREFIX(){
		return GetIniReader()->Get("logger","binlog_prefix","");
	}

	inline const char* SOCKET_PATH(){
		auto ret= GetIniReader()->Get("unix","produce_file","");
		return ret.c_str();
	}
	inline const char* RECV_SOCKET_PATH(){
			auto ret= GetIniReader()->Get("unix","consume_file","");
			return ret.c_str();
		}


	 const int g_fdfs_network_timeout=20;

}
#endif /* SRC_FDFS2QQ_COMMON_DEFINE_H_ */
