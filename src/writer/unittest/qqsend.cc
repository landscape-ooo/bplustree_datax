/*
 * qqsend.cc
 *
 *  Created on: Aug 16, 2016
 *      Author: a11
 */
#include <stdio.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <sstream>

#include "../../box/box_object.h"
#include "writer/TencentStorageServiceWriter.h"

using namespace std;
using namespace store_photo_sdk;
using namespace box::field;
void buildCharlist(std::vector<std::string>& input, std::string& ret) {
	int size = input.size();
	stringstream scout;
	for (std::vector<std::string>::iterator it = input.begin();
			it != input.end(); it++) {
		scout << *it;
		if (it + 1 != input.end())
			scout << BOX_RECORD_SEPERATOR;
	}
	ret.assign(scout.str());
}
///tmp/ssssss2.png /tmp/bd.png  10.126.94.88
std::vector<MessageItem> buildHeader(){
	std::vector<std::string> input= {
			"fileid\tgjfs/123/123/123/jpg",
			"filehandle\t/tmp/bd.png",
			"file_meta\t/tmp/bd.png-m",
			"overwrite_flag\tfalse",
			"category\tsecondmarket",
			"c1\tv1","d2\tv2"};
	string ret;
	buildCharlist(input,ret);
	string body="this is body";
	stringstream ss;
	ss<<ret<<BOX_FIELD_SEPERATOR<<body<<BOX_MSG_SEPERATOR;
	const string origin_msg=ss.str();
	int msgcount=0;
	 std::vector<MessageItem> ret_s=SplitMsg(origin_msg.c_str(),origin_msg.length(),msgcount);
	 return ret_s;
}


int main(int argc, const char *argv[]) {

	if (argc < 4) {
		printf("usage: %s fileid picfile severip\n", argv[0]);
		return 0;
	}

	auto msg=buildHeader();
	writer::tencent::TencentStorageServiceWriter *wr=new  	writer::tencent::TencentStorageServiceWriter;
	auto rsp =wr->messageHeaderFormat(&msg[0])->messageBodyFormat(&msg[0])->messageOutputStream();
	wr->sprintf_response(rsp);
	return 0;
}

