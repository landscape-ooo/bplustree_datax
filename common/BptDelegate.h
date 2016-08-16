/*
 * Fdfs2qqJobschedule.h
 *
 *  Created on: Aug 4, 2016
 *      Author: a11
 */

#ifndef JOBSCHEDULE_FDFS2QQJOBSCHEDULE_H_
#define JOBSCHEDULE_FDFS2QQJOBSCHEDULE_H_
#include <cstdlib>
#include <sstream>
#include "common_define.h"
#include "bpt.h"
#include "tools.h"
namespace jobschedule {
using namespace std;
using namespace fdfs2qq;
class BptDelegate {
public:
	BptDelegate();

	static int InitBptFromFilesource( bpt::bplus_tree* treePtr,const string& ,const int);
	template<typename tkey,typename tvalue>
	static void InsertAndUpdateBptfromData(  bpt::bplus_tree* treePtr,const tkey* keyptr,
			const tvalue * value);


	static void InsertAndUpdateBptfromData( bpt::bplus_tree* treePtr, const string &keyptr,
			const int  value);

	template<typename tkey>
	static bool SearchBptByKey(const bpt::bplus_tree*, const tkey* keyptr,bpt::value_t* &)  ;

//	bpt::bplus_tree** getPlustreeListPtr(int index=0)const {
//		return this->treeListPtr+index;
//	}

private:
//	bpt::bplus_tree *treePtr;
//	bpt::bplus_tree **treeListPtr;
//	Fdfs2qqHelper* helperPtr;

};

}
#endif /* JOBSCHEDULE_FDFS2QQJOBSCHEDULE_H_ */
