/*
 * Fdfs2qqJobschedule.cc
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */
#include "BptDelegate.h"
namespace jobschedule {

int BptDelegate::InitBptFromFilesource(bpt::bplus_tree *treePtr,
		const string& filepath,const int default_value) {
	string tmp;
	std::ifstream t;
	t.open(filepath.c_str(),std::ios::in|std::ios::binary);
	while(std::getline(t,tmp)){
		bpt::key_t _key(tmp.c_str());
		bpt::value_t _value(default_value);
		InsertAndUpdateBptfromData<bpt::key_t, bpt::value_t>(treePtr, &_key,
				&_value);
	}

	return 1;
}

template<typename tkey, typename tvalue>
void BptDelegate::InsertAndUpdateBptfromData(bpt::bplus_tree *treePtr,
		const tkey* keyptr, const tvalue * value) {

	bpt::value_t* _tmp = new int;
	stringstream ss;
	if (SearchBptByKey(treePtr, keyptr, _tmp)) {
		ss << "update..." << std::string(((bpt::key_t*) keyptr)->k, 64)
				<< " old:" << _tmp << " -->:" << *value << std::endl;
		treePtr->update(*keyptr, *value);
	} else {
		ss << "insert into..." << std::string(((bpt::key_t*) keyptr)->k, 64)
				<< " -->:" << *value << std::endl;
		treePtr->insert(*keyptr, *value);
	}
	delete _tmp;
	Logger::info(ss.str());
}
void BptDelegate::InsertAndUpdateBptfromData(bpt::bplus_tree *treePtr,
		const string &keyptr, const int value) {
	bpt::key_t _key(keyptr.c_str());
	bpt::value_t _v(value);

	InsertAndUpdateBptfromData<bpt::key_t, bpt::value_t>(treePtr,&_key,
			&_v);
}

template<typename tkey>
bool BptDelegate::SearchBptByKey(const bpt::bplus_tree* treePtr,
		const tkey* keyptr, bpt::value_t* &value)  {
	auto f = treePtr->search(*keyptr, value);
	if (f != 0) {
		Logger::info(
				"find..." + std::string(((bpt::key_t*) keyptr)->k, 64)
						+ "::NULL.....\n");
		return false;
	}
	Logger::info(
			"find..." + std::string(((bpt::key_t*) keyptr)->k, 64) + "::"
					+ std::to_string(*value) + ".....\n");
	return true;
}
}
