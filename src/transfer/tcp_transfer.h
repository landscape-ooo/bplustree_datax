/*
 * transfer_tcp.h
 *
 *  Created on: Aug 11, 2016
 *      Author: a11
 */

#ifndef SRC_TRANSFER_TRANSFER_TCP_H_
#define SRC_TRANSFER_TRANSFER_TCP_H_

extern "C" {
#include "tcp/connection_pool.h"
#include "tcp/sockopt.h"
}
namespace transfer {
namespace tcp {
class client {
	const static int g_fdfs_network_timeout = 20;
public:
	ConnectionInfo *pCurrentServer;
	static void connect_sever(ConnectionInfo* &pCurrentServer,const string &ip,const string& port) {
		if(!pCurrentServer){
			pCurrentServer = new ConnectionInfo;
		}
		strcpy(pCurrentServer->ip_addr, ip.c_str());
		pCurrentServer->port = std::stoi(port);

		int * err_no = new int;
		*err_no = conn_pool_connect_server(pCurrentServer, g_fdfs_network_timeout);

		return ;
	}

	/**
	 * return :1 success ,-1 fail
	 */
	static int send_req(const ConnectionInfo* pCurrentServer, string &data) {
		int result;
		if ((result = tcpsenddata_nb(pCurrentServer->sock, (char*) data.c_str(),
				data.length(), g_fdfs_network_timeout)) != 0) {
			return -1;
		} else {
			return 1;
		}
	}

	static string recv_rsp(const ConnectionInfo* pCurrentServer) {
		string tmp;
		int max = 1024;
		char data[max];
		int reciv_c = 0;
		tcprecvdata_ex(pCurrentServer->sock, &data, max, 1, &reciv_c);
		string rsp(data, reciv_c);
		return rsp;
	}

	static string Mem2hex(const void *data, const unsigned size)
	{
	    const char *hex = "0123456789ABCDEF";
	    std::string result("");

	    for(unsigned i = 0; i < size; ++i) {
	        unsigned char bytessssss = ((unsigned char *)data)[i];
	        char buffer[2];
	        buffer[0] = hex[bytessssss / 16];
	        buffer[1] = hex[bytessssss % 16];
	        result.append(buffer, sizeof(buffer));
	    }
	    return result;
	}
};
}
}

#endif /* SRC_TRANSFER_TRANSFER_TCP_H_ */
