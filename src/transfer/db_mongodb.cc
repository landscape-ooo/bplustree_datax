/*
 * db_dao.cc
 *
 *  Created on: Oct 10, 2016

 *      Author: a11
 */
#include "db_mongodb.h"
namespace transfer {
namespace dao {




const char * db_mongodb::_db_name ="gj_image";
const char * db_mongodb::_collection_name ="gjfs_table";
const char * db_mongodb::_index_key ="grpid";
const char * db_mongodb::_MONGO_URL_STR="mongodb://10.1.252.164:27017/?minPoolSize=16&connectTimeoutMS=1000";



mongoc_uri_t * db_mongodb::MG_URI_INFO;
mongoc_client_pool_t * db_mongodb::_STATIC_POOL;

//const char * db_mongodb::_db_name =fdfs2qq::MONGODB_DBNAME();
//const char * db_mongodb::_collection_name =fdfs2qq::MONGODB_COLLECTION_NAME();
//const char * db_mongodb::_index_key =fdfs2qq::MONGODB_INDEX();
//const string db_mongodb::_MONGO_URL=fdfs2qq::MONGODB_URL();


void db_mongodb::InitMongoPool(){
	mongoc_init();
	MG_URI_INFO=mongoc_uri_new(_MONGO_URL_STR);
	_STATIC_POOL=mongoc_client_pool_new(MG_URI_INFO);
}
void db_mongodb::GetClientHandle(mongoc_client_t* &t_handle){
	t_handle=mongoc_client_pool_pop(_STATIC_POOL);
}

void db_mongodb::DestoryClientHandle(){
	mongoc_uri_destroy(MG_URI_INFO);
	mongoc_client_pool_destroy(_STATIC_POOL);
}

db_mongodb::db_mongodb(mongoc_client_t* cl){
	_client=cl;
}
db_mongodb::db_mongodb(){
	_client=mongoc_client_pool_pop(_STATIC_POOL);

}

db_mongodb::~db_mongodb(){
	mongoc_client_pool_push(_STATIC_POOL, _client);
}

bool  db_mongodb::isExistByPrimarykey(const char* uniq_id){
	const int limit_c = 1;
	int count;
	char* str;
	int flg;
	bson_error_t error;

	mongoc_collection_t*  local_collection = mongoc_client_get_collection(_client, _db_name, _collection_name);


	bool exist= isExistByPrimarykey(local_collection,uniq_id);
	mongoc_collection_destroy(_collection);

	return exist;
}

bool  db_mongodb::isExistByPrimarykey(mongoc_collection_t* local_collection,const char* uniq_id){
	const int limit_c = 1;
	int count;
	char* str;
	int flg;
	bson_error_t error;


	_query = BCON_NEW(_index_key, BCON_UTF8(uniq_id));

	count = mongoc_collection_count(local_collection, MONGOC_QUERY_NONE, _query, 0,
			limit_c, NULL, /* Read Prefs, NULL for default */
			&error);

	if (count < 0) {
		str = bson_as_json(_query, NULL);
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		" Count failed: %s---->%s  ", __LINE__, error.message, str);

		flg= -1;
	} else if (count == 0) {
		flg= -2;
	} else {
		flg= 1; //exist
	}



	bson_destroy(_query);
	return flg>0?true:false;
}



int db_mongodb::Upsert(const char* uniq_id, const char* value) {
	int ret_flg;
	char* str;
	bson_error_t error;

	_collection = mongoc_client_get_collection(_client, _db_name, _collection_name);



	const bool isexist = this->isExistByPrimarykey(_collection, uniq_id);
	if (isexist) {
		ret_flg = -1;
	} else {
		_insert = BCON_NEW(_index_key, BCON_UTF8(uniq_id));
		char * tmp_value ;
		if (value != NULL && value[0] != '\0') {
			tmp_value =(char*) value;
			BCON_APPEND(_insert, "v", BCON_UTF8(tmp_value));
		}

		if (!mongoc_collection_insert(_collection, MONGOC_INSERT_NONE, _insert,
				NULL, &error)) {
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
			" err in save, %s  ", __LINE__, error.message);
			ret_flg = 0;
		}
		ret_flg = 1;
		bson_destroy(_insert);
	}


	mongoc_collection_destroy(_collection);
	//mongoc_client_destroy (_client);
	return ret_flg;
}
bool db_mongodb::Insert(const char* k, const char* v){
	std::map<string,string> mdic;
	mdic.insert(std::make_pair(std::string(k),std::string(v)));
	return InsertBulk(mdic);
}
bool db_mongodb::InsertBulk(const std::map<string,string>& mdic){
		mongoc_bulk_operation_t *bulk;
		bson_t reply;
		bson_t* doc;
		bson_error_t error;

		_collection = mongoc_client_get_collection(_client, _db_name, _collection_name);
		   /* false indicates unordered */
		bulk = mongoc_collection_create_bulk_operation(_collection, false, NULL);

		for (map<string,string>::const_iterator it =mdic.begin(); it != mdic.end();
				it++) {
			char* uniq_key=(char*)(it->first.c_str());
			char* tmp_v=(char*)(it->second.c_str());
			doc = BCON_NEW(_index_key, BCON_UTF8 (uniq_key));
			BCON_APPEND(doc, "v", BCON_UTF8(tmp_v));
			mongoc_bulk_operation_insert(bulk, doc);

			bson_destroy(doc);
		}

		bool ret=mongoc_bulk_operation_execute (bulk, &reply, &error);
		if(!ret) {
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
					" err in bulk, %s  ", __LINE__, error.message);
		}
		char *str = bson_as_json (&reply, NULL);
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
						"total %d , relay %s  ", __LINE__, mdic.size(), str);

		mongoc_bulk_operation_destroy(bulk);
		mongoc_collection_destroy(_collection);



		sleep(2);


}
bool db_mongodb::InsertBulk(const char** dic,const int len) {

}



}
}

