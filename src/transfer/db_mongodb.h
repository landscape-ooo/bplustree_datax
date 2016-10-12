/*
 * db_dao.h
 *
 *  Created on: Oct 10, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_TRANSFER_DB_DAO_H_
#define SRC_SRC_TRANSFER_DB_DAO_H_
//#ifdef __cplusplus
//extern "C" {
//#endif
#include <mongoc.h>
#include <map>
#include <string>

#include "common/common_define.h"
#include "fastdfs/common/logger.h"
//#ifdef __cplusplus
//}
//#endif
using namespace std;
namespace transfer {
namespace dao {

class db_mongodb {

public:
	static void InitMongoPool();
	static void GetClientHandle(mongoc_client_t* &t_handle);

	static void DestoryClientHandle();

private:
	static 	mongoc_client_pool_t *_STATIC_POOL;
	static const char* _MONGO_URL_STR;
	static mongoc_uri_t *MG_URI_INFO;
public:
	db_mongodb(mongoc_client_t* );
	db_mongodb();
	~db_mongodb();
	/**
	 * if not exist ,insert
	 *    if exist
	 * @return
	 *  -1 exist
	 *  0 error
	 *  1 success ,new one
	 */
	int Upsert(const char* k, const char* v);

	/**
	 * just insert
	 */
	bool Insert(const char* k, const char* v);
	/**
	 * @param
	 *
	 */
	bool InsertBulk(const char** dic,const int len);
	bool InsertBulk(const std::map<string,string>& mdic);

	/**
	 * @return
	 */
	bool  isExistByPrimarykey(mongoc_collection_t*,const char* );
	/**
	 * @return
	 */
	bool isExistByPrimarykey(const char*);


private:
	mongoc_client_t *_client;
	mongoc_collection_t *_collection;

	static const char *_db_name ;
	static const char *_collection_name ;
	static const char *_index_key ;


	bson_t *_query, *_insert;


};

}
}
#endif /* SRC_SRC_TRANSFER_DB_DAO_H_ */
