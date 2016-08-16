/*
 * test.connctpool.cc
 *
 *  Created on: Aug 11, 2016
 *      Author: a11
 */
#include <iostream>
extern "C"{
#include "tcp/connection_pool.h"
#include "tcp/sockopt.h"
}

int main (){

	ConnectionInfo *pCurrentServer=new ConnectionInfo;
//	const std::string ipstr="127.0.0.1";
//	pCurrentServer->ip_addr(ipstr.c_str());
	strcpy(pCurrentServer->ip_addr, "192.168.2.188");
	pCurrentServer->port=8081;

	int * err_no=new int ;
	const int connect_timeout=20;
	const int g_fdfs_network_timeout=20;
	*err_no = conn_pool_connect_server(pCurrentServer, \
				connect_timeout);

	const int size=100;
	char header[size]={""};

	int result;
	if ((result=tcpsenddata_nb(pCurrentServer->sock, &header, \
				sizeof(header), g_fdfs_network_timeout)) != 0){
		std::cout<<"error!!!";
	}else{
		std::cout<<"success";
	}


}

