/*
 * test_fdfshelper.cc
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */
#include "../Fdfs2qqHelper.h"
#include <gtest/gtest.h>

using namespace std;
using namespace fdfs2qq;
//TEST(Fdfs2qqHelper, buildPath) {
//	Fdfs2qqHelper* s=new Fdfs2qqHelper("/data/log/fdfs2qq","gjfs01",10);
//	map <string,std::vector<std::string> > ret;
//
//	s->getstoreprefix();
//	s->get_store_prefix_list(ret);
//	EXPECT_EQ(10,ret.size());
//
//	std::vector<string> keys;
//
//	auto g=ret["/data/log/fdfs2qq/gjfs01_M00"];
//
//	EXPECT_EQ(256*256*2,g.size());
//
//}
TEST(Fdfs2qqHelper,readbuge) {
	std::string path("/data/log/fdfs2qq/gjfs01_M01_FF_FF_finished_success.log");
	Fdfs2qqHelper* s=new Fdfs2qqHelper("/data/log/fdfs2qq","gjfs01",10);

	concurrent_queue<string>*   queuePtr=new concurrent_queue<string>;
	auto numline=s->Readhugefile(path.c_str(),queuePtr);
	uintmax_t se=10;
	EXPECT_EQ(se,numline);

}

