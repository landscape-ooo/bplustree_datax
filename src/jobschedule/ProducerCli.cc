/*
 * CliProducer.cc
 *
 *  Created on: Aug 25, 2016
 *      Author: a11
 */
#include "ProducerCli.h"
namespace jobschedule {
const string ProducerCli::NULL_MSG_SIGNAL = "NULL MQ,NEED PUT MSG";
//::pthread_mutex_t ProducerCli::G_bplus_tree_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;
::pthread_mutex_t ProducerCli::G_produce_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;
::pthread_cond_t ProducerCli::G_Condition_variable;



ConnectionInfo* ProducerCli::pTrackerServer=new ConnectionInfo;
ConnectionInfo* ProducerCli::pConsumerServer=new ConnectionInfo;
const char * ProducerCli::_notify_socket_path = fdfs2qq::SOCKET_PATH();
const char * ProducerCli::item_socket_path = fdfs2qq::RECV_SOCKET_PATH();

bpt::bplus_tree ** ProducerCli::G_BptList;


int ProducerCli::MAX_CONSUME_THREAD_NUM = fdfs2qq::GetCpuCoreCount;
int ProducerCli::MAX_PRODUCER_THREAD_NUM = fdfs2qq::GetCpuCoreCount;

fdfs2qq::concurrent_queue<string> ProducerCli::G_ItemProduce_Mq;
fdfs2qq::concurrent_queue<StorageVolumnObject> ProducerCli::G_Volumns_Mq;

void ProducerCli::Init() {
//	pTrackerServer = new ConnectionInfo;
	strcpy(pTrackerServer->ip_addr, fdfs2qq::TRACKER_IP().c_str());
	pTrackerServer->port = fdfs2qq::TRACKER_PORT();

//	pConsumerServer = new ConnectionInfo;
	strcpy(pConsumerServer->ip_addr, fdfs2qq::CONSUME_IP().c_str());
	pConsumerServer->port = fdfs2qq::CONSUME_PORT();

}

int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data) {
	int result ;
	if((result=tcpsenddata_nb(pCurrentServer->sock, (void*) data.c_str(),
			data.length(), g_fdfs_network_timeout)) != 0) {
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		"sync appender file, tcpsenddata_nb: %s error, "
		"maybe disconnect? later? reason status %d :%s,",
		__LINE__, data.c_str(), result,
		STRERROR(result));
	}
	return result;
}
int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data, RESPONSE_HEADER &ret) {
	const int g_fdfs_network_timeout = 20;

	const int connect_timeout = 20;
	int errorno=0;
	if((errorno = conn_pool_connect_server(pCurrentServer, connect_timeout))!= 0) {
				fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"tcpsenddata_nb connect error, "
				"maybe disconnect? later? reason status %d :%s,",
				__LINE__,  errorno,
				STRERROR(errorno));
	}

	int result = TransferByData(pCurrentServer, data);

	RESPONSE_HEADER resp_tmp;

	if ((result = tcprecvdata_nb(pCurrentServer->sock, &resp_tmp, sizeof(RESPONSE_HEADER),
			g_fdfs_network_timeout)) != 0) {
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		"tracker server %s:%d, recv data fail, "
		"errno: %d, error info: %s.",
		__LINE__, pCurrentServer->ip_addr, pCurrentServer->port, result,
				STRERROR(result));
	}
	memcpy(&ret, &resp_tmp, sizeof(RESPONSE_HEADER));
	return result;
}

void ProducerCli::TransferByFilename(ConnectionInfo* pCurrentServer,
		const string file_name) {
	struct stat stat_buf;
	if (lstat(file_name.c_str(), &stat_buf) != 0) {
		if (errno == ENOENT) {
//			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
//			"sync appender file, file: %s not exists, "
//			"maybe deleted later?",
//			__LINE__, file_name.c_str());

			return;
		} else {
			int result = errno != 0 ? errno : EPERM;
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
			"call stat fail, appender file: %s, "
			"error no: %d, error info: %s",
			__LINE__, file_name.c_str(), result, STRERROR(result));
			return;
		}
	}

	string tmp;
	std::ifstream t;
	t.open(file_name.c_str(), std::ios::in | std::ios::binary);


	const int g_fdfs_network_timeout = 20;
	const int connect_timeout = 20;
	int errorno;
	if((conn_pool_connect_server(pCurrentServer, connect_timeout)) != 0) {
				fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"tcpsenddata_nb connect error, "
				"maybe disconnect? later? reason status %d :%s,",
				__LINE__,  errorno,
				STRERROR(errorno));
	}

	while (std::getline(t, tmp)) {
		tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
		stringstream sm;
		sm << fdfs2qq::CMD::LSM << fdfs2qq::BOX_MSG_SEPERATOR << tmp << "\n";
		tmp = sm.str();
		//fdfs2qq::Logger::info(tmp);
		int result = TransferByData(pCurrentServer, tmp);
		if (result != 0) {
			fdfs2qq::Logger::error("send file fail,msg %s,reason %s",tmp.c_str(),STRERROR(result));
		}
	}

}
void ProducerCli::LSMBinlog() {

	//
	Init();
	//simulat all filepath

	fdfs2qq::Logger::debug("start cal keylist");

	auto ret = StorageConfig::VolumnsDict;

	//foreach key ,init a tree
	stringstream out;
	out << "ret.size()" << ret.size() << "\n";
	fdfs2qq::Logger::info(out.str());

	int index = 0;

	//end transfer

	for (std::map<string, string>::iterator i = ret.begin(); i != ret.end();
			++i) {
		int index_map = fdfs2qq::String2int(i->first);

		std::vector<StorageVolumnObject> sublist;
		StorageConfig::getStoreFolderByOrder(sublist, i->first);
		for (std::vector<StorageVolumnObject>::iterator i_list =
				sublist.begin(); i_list != sublist.end(); i_list++) {
			string target = i_list->grpid + "_" + i_list->volumnid + "_"
					+ i_list->subdir + "_finished_";
			while (target.find("/") != std::string::npos) {
				target = target.replace(target.find("/"),
						std::string("/").length(), "_");
			}

			string logsuccess = fdfs2qq::LOGPREFIX() + "/" + target
					+ "success.log";

			TransferByFilename(pTrackerServer, logsuccess);

		}
	}
	char buffer[100];
	string formater = "build finish ...total valid filecout=%d ";
	int n_len = sprintf(buffer, formater.c_str(), index);
	fdfs2qq::Logger::info(std::string(buffer, n_len));
}

void ProducerCli::ReadyProducer() {

	std::vector<fdfs2qq::StorageVolumnObject> v;
	std::map<std::string, std::string> p(StorageConfig::VolumnsDict);
//	for (auto item : StorageConfig::VolumnsDict) {
	for(std::map<std::string, std::string>::iterator item=p.begin();item!=p.end();item++){

		string tmp(item->first);
		StorageConfig::getStoreFolderByOrder(v, tmp);
	}
	//set queue
	for (std::vector<fdfs2qq::StorageVolumnObject>::iterator it = v.begin();
			it != v.end(); it++) {
		pthread_mutex_lock(&G_produce_Ptrmutex);
		//G_Volumns_Mq.push(it->to_string());
		G_Volumns_Mq.push(*it);
		pthread_mutex_unlock(&G_produce_Ptrmutex);
	}

}

/****
 * producer mq start
 */

//once
void ProducerCli::ForkProducer() {
//		string str;
//				G_Volumns_Mq.try_dequeue(str);
//		fdfs2qq::StorageVolumnObject ori_obj=StringToStorageVolumnObject(str);
//		_ProducerMsg(ori_obj);

	//pool
	int max_c = MAX_PRODUCER_THREAD_NUM;
	pthread_t pool[max_c];
	Logger::debug("produce thread num" + fdfs2qq::to_string(max_c));

	for (int i = 0; i < max_c; i++) {
		::pthread_create(&pool[i], NULL, &ProducerCli::WaitProducer, NULL);
		::pthread_detach(pool[i]);
	}

}

void* ProducerCli::WaitProducer(void*) {
	while (true) {
		bool flg=false;
		StorageVolumnObject ori_obj;
		if(!G_Volumns_Mq.empty()){
			flg=G_Volumns_Mq.try_pop(ori_obj);
		}
		if(ori_obj.isvalid){
				//fdfs2qq::StorageVolumnObject ori_obj=StringToStorageVolumnObject(str);
				_ProducerMsg(ori_obj);
		}
	}
}
void ProducerCli::_ProducerStopMsg() {
//	}
}

void ProducerCli::_ProducerMsg(
		const fdfs2qq::StorageVolumnObject &storageInfo) {
	//ready mutex  -->is_finished_hash

	vector<string> filelist, globalid_list;
	StorageConfig::GetfileListOfStoragepath(
			storageInfo.volumnstr + "/" + storageInfo.subdir, filelist);

	StorageConfig::GetGlobalIdByFilelist(filelist, storageInfo, globalid_list);

	string volumns_str = storageInfo.volumnid;
	volumns_str.erase(
			remove_if(volumns_str.begin(), volumns_str.end(),
					not1(ptr_fun(static_cast<int (*)(int)>(isdigit)))), volumns_str.end()
	);
	const int volumns_id_int = std::atoi(volumns_str.c_str());

	for (std::vector<string>::iterator it = globalid_list.begin();
			it != globalid_list.end(); it++) {

		if (volumns_id_int < fdfs2qq::StorageConfig::MAX_VOLUMNS_COUNT) {
			string tmp(*it);
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
			stringstream sm;
			sm << fdfs2qq::CMD::REGIST
					<< fdfs2qq::BOX_MSG_SEPERATOR<<storageInfo.volumnstr
					<<fdfs2qq::BOX_MSG_SEPERATOR<< tmp
					<< "\n";
			tmp = sm.str();
			const size_t bodysize=sizeof(RESPONSE_HEADER);

			RESPONSE_HEADER header;
			int result = TransferByData(pTrackerServer, tmp, header);
			auto stssatus =String2int(std::string(header.ext_status));
			if ( stssatus== RESPONSE_STATUS::REGISTSUCESS) {
				fdfs2qq::Logger::info("send tracker fileid:%s ,response status :%s",tmp.c_str(),header.ext_status);


				fdfs2qq::StorageFileObject fileobj;
				try{
					fileobj=ResponseObject2StorageFileObject(header);
					G_ItemProduce_Mq.push(fileobj.global_fileid);
				}catch(std::exception &ex){

					fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
					"catch exception ,status :%s ,fileid:%s ,reason:%s ",
					__LINE__, header.ext_status,header.fileid,ex.what());
				}
		//end
			} else {
				fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
							"depelated, filid :%s ",
							__LINE__, header.fileid);
			}
		} else {
			fdfs2qq::Logger::error(
					*it + ",\tfail index G_BptList,for index=" + volumns_str);
		}
	}

}

//
void ProducerCli::ForkConsumer() {

	int max_c = MAX_CONSUME_THREAD_NUM;

	Logger::debug("consume thread num" + fdfs2qq::to_string(max_c));
	pthread_t pool[max_c];
	//pthread_t log_thread;
	for (int i = 0; i < max_c; i++) {
		::pthread_create(&pool[i], NULL, &ProducerCli::ListenItemConsumerMq,
				NULL);
		::pthread_detach(pool[i]);
	}
}

void* ProducerCli::ListenItemConsumerMq(void*) {
	while (true) {
		bool flg = false;
		string fileid;
		if (!G_ItemProduce_Mq.empty()) {
			flg = G_ItemProduce_Mq.try_pop(fileid);
			if (flg&&!fileid.empty()) {
				fdfs2qq::Logger::info("will consume send file:%s",fileid.c_str());
				TransferByData(pConsumerServer, fileid);
			}
		}

	}
}


}







int main(int argc, const char *argv[]) {
	struct FastLogStat logstat = { kLogAll, kLogFatal, kLogSizeSplit };
	FastLog::OpenLog(fdfs2qq::LOGPREFIX().c_str(), "fdfs2qq_produce", 2048, &logstat,
			NULL);

	jobschedule::ProducerCli::LSMBinlog();
	//fork producer
	jobschedule::ProducerCli::ReadyProducer();
	jobschedule::ProducerCli::ForkProducer();
	//
	////		ProducerCli::Init();
	////		//fork producer
	jobschedule::ProducerCli::ForkConsumer();

	return 1;
}
