/*
 * unittest.box.cc
 *
 *  Created on: Aug 11, 2016
 *      Author: a11
 */
#include <typeinfo>
#include <gtest/gtest.h>

#include "box_object.h"
using namespace box::field;

void buildCharlist(std::vector<std::string>&, string &);

TEST(SquareRootTest, PositiveNos) {
	Header* s=new Header();
	HeaderItem* sitem=new HeaderItem();
	s->setHeader(sitem); //not set isvalid
	EXPECT_EQ(0,s->_MaxPtrLen);
	HeaderItem* sitem2=sitem;
	s->setHeader(sitem2);//not set isvalid
	EXPECT_EQ(0,s->_MaxPtrLen);

	sitem->isvalid=true;
	s->setHeader(sitem);
	EXPECT_EQ(1,s->_MaxPtrLen);
	s->setHeader(sitem2);
	EXPECT_EQ(2,s->_MaxPtrLen);

}

const int max_value_size = 7;
TEST(SquareRootTest, split) {
	Header* s=new Header();

	std::vector<std::string> input= {"k\tv","c1\tv1","d2\tv2"};
	string ret;
	buildCharlist(input,ret);

	auto shar=splitMetalist(ret.c_str(),ret.length());
	{
		auto ptrvalue=&(shar[1]);
		string key(ptrvalue->key_v);
		string value(ptrvalue->value_v);
		EXPECT_EQ(key,"c1");
		EXPECT_EQ(value,"v1");
	}

	{
		auto ptrvalue=&(shar[2]);
		string key(ptrvalue->key_v);
		string value(ptrvalue->value_v);
		EXPECT_EQ(key,"d2");
		EXPECT_EQ(value,"v2");
	}
	{
		EXPECT_EQ(3,shar.size());
	}
}

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

TEST(box_field,_GetOccurCount) {
	const string str1="this is sample\tthis is a sample\t";
	const string str2="this is sample\tthis is a sample";
	auto count=_GetOccurCount(str1,'\t');
	EXPECT_EQ(2,count);

	count=_GetOccurCount(str2,'\t');
	EXPECT_EQ(1,count);


	EXPECT_EQ(_GetOccurCount(str1,'\t'),
			_GetOccurCount(str1.c_str(),str1.length(),'\t')
	);

	EXPECT_EQ(_GetOccurCount(str2,'\t'),
			_GetOccurCount(str2.c_str(),str2.length(),'\t')
	);
}
TEST(box_field,SplitMsg){
	std::vector<std::string> input= {"k\tv","c1\tv1","d2\tv2","s\tfilehandle"};
		//std::vector<std::string> input= {"k\tv","c1\tv1","d2\tv2","filehandle\tfilehandle"};
	string ret;
	buildCharlist(input,ret);
	string body="this is body";
	stringstream ss;
	ss<<ret<<BOX_FIELD_SEPERATOR<<body<<BOX_MSG_SEPERATOR;
	const string origin_msg=ss.str();

	int msgcount=0;
	 std::vector<MessageItem> ret_s=SplitMsg(origin_msg.c_str(),origin_msg.length(),msgcount);

	EXPECT_EQ(msgcount,1);
	EXPECT_EQ(ret_s.size(),1);
	EXPECT_EQ(ret_s[0].hd_ptr._MaxPtrLen,4);
	auto headitem=*(ret_s[0].hd_ptr._Hptr+2);
	EXPECT_EQ(std::string(headitem.key_v),"d2");
}
