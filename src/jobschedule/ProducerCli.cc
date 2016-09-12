/*
 * CliProducer.cc
 *
 *  Created on: Aug 25, 2016
 *      Author: a11
 */
#include "ProducerCli.h"
namespace jobschedule {
const string ProducerCli::NULL_MSG_SIGNAL = "NULL MQ,NEED PUT MSG";
::pthread_mutex_t ProducerCli::G_bplus_tree_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;
::pthread_mutex_t ProducerCli::G_produce_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;
ConnectionInfo* ProducerCli::pTrackerServer;
const char * ProducerCli::_notify_socket_path = fdfs2qq::SOCKET_PATH();
const char * ProducerCli::item_socket_path = fdfs2qq::RECV_SOCKET_PATH();

//::pthread_cond_t ProducerCli::G_Condition_variable;
bpt::bplus_tree ** ProducerCli::G_BptList;

int ProducerCli::MAX_PRODUCER_THREAD_NUM = fdfs2qq::GetCpuCoreCount;

fdfs2qq::concurrent_queue<fdfs2qq::StorageVolumnObject> ProducerCli::G_Volumns_Mq;
void ProducerCli::Init() {
	pTrackerServer = new ConnectionInfo;
	strcpy(pTrackerServer->ip_addr, fdfs2qq::TRACKER_IP().c_str());
	pTrackerServer->port = fdfs2qq::TRACKER_PORT();
}

int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data) {
	const int connect_timeout = 20;
//	const int g_fdfs_network_timeout = 20;
	int * err_no = new int;
	*err_no = conn_pool_connect_server(pCurrentServer, connect_timeout);

	int result = tcpsenddata_nb(pCurrentServer->sock, (void*) data.c_str(),
			data.length(), g_fdfs_network_timeout);
	if (result != 0) {
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		"sync appender file, tcpsenddata_nb: %s error, "
		"maybe disconnect? later?",
		__LINE__, data.c_str());
	}
}
int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data, string& response) {
	const int g_fdfs_network_timeout = 20;

	int result = TransferByData(pCurrentServer, data);
	size_t relen = sizeof(RESPONSE_HEADER);
	RESPONSE_HEADER resp;
	memset(&resp, 0, sizeof(resp));

	if ((result = tcprecvdata_nb(pCurrentServer->sock, &resp, sizeof resp,
			g_fdfs_network_timeout)) != 0) {
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		"tracker server %s:%d, recv data fail, "
		"errno: %d, error info: %s.",
		__LINE__, pCurrentServer->ip_addr, pCurrentServer->port, result,
				STRERROR(result));
	}
	response.resize(relen);
	memcpy(&response, &resp, relen);
	return result;
}

void ProducerCli::TransferByFilename(ConnectionInfo* pCurrentServer,
		const string file_name) {
	struct stat stat_buf;
	if (lstat(file_name.c_str(), &stat_buf) != 0) {
		if (errno == ENOENT) {
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
			"sync appender file, file: %s not exists, "
			"maybe deleted later?",
			__LINE__, file_name.c_str());

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
	while (std::getline(t, tmp)) {
		tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
		stringstream sm;
		sm << fdfs2qq::CMD::LSM << fdfs2qq::BOX_MSG_SEPERATOR << tmp << "\n";
		tmp = sm.str();
		fdfs2qq::Logger::info(tmp);
		int result = TransferByData(ProducerCli::pTrackerServer, tmp);
		if (result != 0) {
			fdfs2qq::Logger::info("send file fail");
		}
	}

}
void ProducerCli::LSMBinlog() {

	//simulat all filepath

	fdfs2qq::Logger::debug("start cal keylist");

	auto ret = StorageConfig::VolumnsDict;

	//foreach key ,init a tree
	stringstream out;
	out << "ret.size()" << ret.size() << "\n";
	fdfs2qq::Logger::info(out.str());

	int index = 0;
	//transfer
//	ConnectionInfo* pCurrentServer = new ConnectionInfo;
//	strcpy(pCurrentServer->ip_addr, "127.0.0.1");
//	pCurrentServer->port = 45845;

	//end transfer

	for (std::map<string, string>::iterator i = ret.begin(); i != ret.end();
			++i) {
		int index_map = fdfs2qq::String2int(i->first);
//		treePtr = *(G_BptList + index_map); // pointer to current tree

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
//			string logerror = fdfs2qq::LOGPREFIX + "/" + target + "error.log";
//			int value = -1; //error

//			if (BptDelegate::InitBptFromFilesource(treePtr, logerror, value)
//					> 0) {
//				index++;
//			}

			string logsuccess = fdfs2qq::LOGPREFIX() + "/" + target
					+ "success.log";
//			value = 1; //default success
//			if (BptDelegate::InitBptFromFilesource(treePtr, logsuccess, value)
//					> 0) {
//				index++;
//			}

			TransferByFilename(pTrackerServer, logsuccess);

		}
	}
	char buffer[100];
	string formater = "build finish ...total valid filecout=%d ";
	int n_len = sprintf(buffer, formater.c_str(), index);
	fdfs2qq::Logger::info(std::string(buffer, n_len));
}

void ProducerCli::ReadyProducer() {
//	fdfs2qq::StorageConfig * objptr = new fdfs2qq::StorageConfig();

	std::vector<fdfs2qq::StorageVolumnObject> v;
	for (auto item : StorageConfig::VolumnsDict) {
		string tmp(item.first);
		StorageConfig::getStoreFolderByOrder(v, tmp);
	}
	//set queue
	for (std::vector<fdfs2qq::StorageVolumnObject>::iterator it = v.begin();
			it != v.end(); it++) {
		G_Volumns_Mq.push(*it);
	}

//	delete objptr;
}

/****
 * producer mq start
 */

//once
void ProducerCli::ForkProducer() {

	_ProducerMsg(G_Volumns_Mq.try_pop());

	//pool
	int max_c = MAX_PRODUCER_THREAD_NUM;
	pthread_t pool[max_c];
	Logger::debug("produce thread num" + std::to_string(max_c));

	for (int i = 0; i < max_c; i++) {
		::pthread_create(&pool[i], NULL, &ProducerCli::WaitProducer, NULL);
		::pthread_detach(pool[i]);
	}

}

void* ProducerCli::WaitProducer(void*) {

	std::string s;
	s.clear();

	while (true) {
		pthread_mutex_lock(&G_produce_Ptrmutex);
		pthread_cond_wait(&fdfs2qq::G_Condition_variable, &G_produce_Ptrmutex);
		_ProducerMsg(G_Volumns_Mq.try_pop());
		pthread_mutex_unlock(&G_produce_Ptrmutex);

		if (G_Volumns_Mq.empty()) {
			perror("exit,for produce finished \n");
			_ProducerStopMsg();
			pthread_exit(NULL); // producer exit
		}

	}
}
void ProducerCli::_ProducerStopMsg() {
//	fdfs2qq::OBJECT_ID_4FILE stopmsg;
//	stopmsg.IS_PROGRAME_THREAD_STOP=true;
//	for(int i=0;i<10;i++){
//		fdfs2qq::G_ItemProduce_Mq.push(stopmsg);
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
			sm << fdfs2qq::CMD::REGIST << fdfs2qq::BOX_MSG_SEPERATOR << tmp
					<< "\n";
			tmp = sm.str();
			fdfs2qq::Logger::info(tmp);
			string resp;
			int result = TransferByData(pTrackerServer, tmp, resp);
			RESPONSE_HEADER header = StringToResponseObject(resp);
//			memcpy(&header,&resp,resp.length());

			if (stoi(header.ext_status) == RESPONSE_STATUS::NEWONE) {
				fdfs2qq::StorageFileObject fileobj=ResponseObject2StorageFileObject(header);
				fdfs2qq::G_ItemProduce_Mq.push(fileobj);

//				_SendUnixDomain(header);
			} else {
				fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"tracker server %s:%d, recv data fail, "
				"errno: %d, error info: %s, recv info :%s",
				__LINE__, pTrackerServer->ip_addr, pTrackerServer->port, result,
						STRERROR(result), resp.c_str());
			}
		} else {
			fdfs2qq::Logger::error(
					*it + ",\tfail index G_BptList,for index=" + volumns_str);
		}
	}

}

}
