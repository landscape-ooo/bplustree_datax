/*
 * test_fdfshelper.cc
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */
#include <gtest/gtest.h>
#include "../BptDelegate.h"

using namespace std;
using namespace fdfs2qq;
using namespace jobschedule;
TEST(Fdfs2qqHelper,readbuge) {
	BptDelegate* s=new BptDelegate();
	s->initBpt();

	auto bptlist_ptr=s->getPlustreeListPtr(1);
	//from error
	std::string tmp("gjfs01/M01/FF/FF/CgEHkVWhOJK30U,oAABxuKsLzlE250.jpg");
	bpt::key_t _key(tmp.c_str());
	bpt::value_t* _v=new int;

	EXPECT_EQ(true,s->searchBptByKey<bpt::key_t>(*bptlist_ptr,static_cast<const bpt::key_t* >(&_key),_v));
	EXPECT_EQ(*_v,1);

	tmp="gjfs01/M01/FF/FF/wKhxwE44dCfvVigIAACdrxSTfhI519.jpg";
	*_v=-100;
	bpt::key_t _key2(tmp.c_str());
	EXPECT_EQ(true,s->searchBptByKey<bpt::key_t>(*bptlist_ptr,&_key2,_v)) ;
	EXPECT_EQ(*_v,1);

	tmp="gjfs01/M01/FF/FF/notexists.jpg";
	*_v=-100;
	bpt::key_t _key3(tmp.c_str());
	EXPECT_EQ(false,s->searchBptByKey<bpt::key_t>(*bptlist_ptr,&_key3,_v));

}

