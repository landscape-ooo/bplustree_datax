/*
 * test.storageconf.cc
 *
 *  Created on: Aug 24, 2016
 *      Author: a11
 */

#include "gtest/gtest.h"
#include "box/StorageConfig.h"
#include <fstream>
#include <cstdlib>
using namespace std;
using namespace  fdfs2qq;


TEST(storage,getfileListOfStoragepath) {
	const string prefix="/tmp/unitest/";
	::system("mkdir -p /tmp/unitest/");
	const vector<string> image_mock_filelist={"a.jpg","b.png","c.jpeg","d.xxxx"};
	//mock
	for(string img:image_mock_filelist) {
		fstream fs;
		fs.open(prefix+img, ios::out);
		fs.close();
	}

	auto vals=fdfs2qq::StorageConfig::GetVolumnsDict();
	//test
	std::vector<string> result;
	auto flg=fdfs2qq::StorageConfig::GetfileListOfStoragepath(prefix,result);
	EXPECT_TRUE(flg);
	EXPECT_EQ(3,result.size());


	//test globalid
	vector<string> expect={"gjfs09/M00/29/00/a.jpg","gjfs09/M00/29/00/b.png","gjfs09/M00/29/00/c.jpeg"};

	fdfs2qq::StorageVolumnObject obj;
	obj.grpid=StorageConfig::GROUP_ID;
	obj.volumnid="M00";
	obj.volumnstr=StorageConfig::VolumnsDict[obj.volumnid];
	obj.subdir="29/00";


	vector<string> g_fileid_list;
	fdfs2qq::StorageConfig::GetGlobalIdByFilelist(\
			result,obj,g_fileid_list
	);
	auto v1=g_fileid_list;
	std::vector<string> v_intersection;

	   std::sort(v1.begin(), v1.end());
	    std::sort(expect.begin(), expect.end());


	std::set_intersection(v1.begin(), v1.end(),
	expect.begin(), expect.end(),
	std::back_inserter(v_intersection));

	EXPECT_EQ(v_intersection,expect);
}




TEST(storage,getStoreFolderByOrder) {



	auto vals=fdfs2qq::StorageConfig::GetVolumnsDict();
	string volumnsid="M0";
	std::vector<StorageVolumnObject> ret;
	fdfs2qq::StorageConfig::getStoreFolderByOrder(ret,volumnsid);
	//size
	EXPECT_EQ(256,ret.size());
	//item
//	for(auto imap= ret.begin();imap!=ret.end();imap++){
//		string key=*imap;
//		EXPECT_TRUE(key.find(prefix)!=std::string::npos);
//	}
}





TEST(storage,get) {

	EXPECT_EQ(5,fdfs2qq::StorageConfig::MAX_VOLUMNS_COUNT);

	EXPECT_EQ("gjfs09",fdfs2qq::StorageConfig::GROUP_ID);

}


TEST(storage,GetVolumnsDict) {

	auto vals=fdfs2qq::StorageConfig::GetVolumnsDict();
	for(auto imap: vals){
		string key=imap.first;
		EXPECT_TRUE(key.find("M")!=std::string::npos);
	}

}
