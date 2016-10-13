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
#include <map>
#include "common_define.h"
#include "transfer/db_mongodb.h"
#include "tools.h"
namespace jobschedule {
using namespace std;
using namespace fdfs2qq;
class MongoDelegate {
public:
	MongoDelegate();

	static int InitBptFromFilesource( );

	static bool InsertAndUpdateBptfromData( const string& keyptr,
			const int value);
	static bool InsertAndUpdateBptfromData( const map<string,string>&);

	static bool SearchBptByKey(const string&)  ;



};

}
#endif /* JOBSCHEDULE_FDFS2QQJOBSCHEDULE_H_ */
