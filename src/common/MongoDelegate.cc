/*
 * Fdfs2qqJobschedule.cc
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */
#include "MongoDelegate.h"
using namespace transfer::dao;
namespace jobschedule {

int MongoDelegate::InitBptFromFilesource() {
	db_mongodb::InitMongoPool();
	return 1;
}

void MongoDelegate::InsertAndUpdateBptfromData(const string &keyptr,
		const int value) {

	auto value_t = fdfs2qq::to_string(value);
	db_mongodb* ptr = new db_mongodb();
	int ret_flg = -1;
	ret_flg = ptr->Upsert(keyptr.c_str(), value_t.c_str());

	delete ptr;
}

bool MongoDelegate::SearchBptByKey(const string& keyptr) {
	db_mongodb* ptr = new db_mongodb();
	bool ret_flg = ptr->isExistByPrimarykey(keyptr.c_str());
	delete ptr;
	return ret_flg;
}
}
