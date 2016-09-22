/*
 * unixdomain.h
 *
 *  Created on: Sep 7, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_TRANSFER_UNIXDOMAIN_H_
#define SRC_SRC_TRANSFER_UNIXDOMAIN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fastdfs/common/sockopt.h"

#include "fastdfs/common/logger.h"
namespace transfer {
namespace tcp {


inline void SendByUnixDomain(const char* socket_path, const string& data,const int g_fdfs_network_timeout=20) {
	ConnectionInfo* ptr=new ConnectionInfo;

	strncpy(ptr->socket_path,socket_path,sizeof(ptr->socket_path)-1);

	int result=conn_pool_connect_unixdomain(ptr,g_fdfs_network_timeout);

	result = tcpsenddata(ptr->sock, (void*) data.c_str(), data.length(),
			g_fdfs_network_timeout);

	delete ptr;
	if (result != 0) {
		fdfs2qq::Logger::error("fail connect to send unixdomain");
	}
}

//inline void SendByUnixDomain(const char* socket_path, const string& data,const int g_fdfs_network_timeout=20) {
//	int socketlisten;
//	struct sockaddr_un addresslisten;
//	socketlisten = socket(AF_UNIX, SOCK_STREAM, 0);
//	if (socketlisten < 0) {
//		char buffer[50];
//		int len=sprintf(buffer, "socket() failed: %s\n", strerror(errno));
//		throw std::runtime_error("Failed to create listen socket;"+std::string(buffer,len));
//	}
//
//	memset(&addresslisten, 0, sizeof(addresslisten));
//	addresslisten.sun_family = AF_UNIX;
//	if (*socket_path == '\0') {
//		*addresslisten.sun_path = '\0';
//		strncpy(addresslisten.sun_path + 1, socket_path + 1,
//				sizeof(addresslisten.sun_path) - 2);
//	} else {
//		strncpy(addresslisten.sun_path, socket_path,
//				sizeof(addresslisten.sun_path) - 1);
//	}
//	unlink(socket_path);
//
//	int result = tcpsenddata(socketlisten, (void*) data.c_str(), data.length(),
//			g_fdfs_network_timeout);
//	if (result != 0) {
//		fdfs2qq::Logger::error("fail connect to send unixdomain");
//	}
//}

}
}

#endif /* SRC_SRC_TRANSFER_UNIXDOMAIN_H_ */
