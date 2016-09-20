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
ConnectionInfo* ProducerCli::pTrackerServer;
const char * ProducerCli::_notify_socket_path = fdfs2qq::SOCKET_PATH();
const char * ProducerCli::item_socket_path = fdfs2qq::RECV_SOCKET_PATH();

::pthread_cond_t ProducerCli::G_Condition_variable;
bpt::bplus_tree ** ProducerCli::G_BptList;


int ProducerCli::MAX_CONSUME_THREAD_NUM = fdfs2qq::GetCpuCoreCount;
int ProducerCli::MAX_PRODUCER_THREAD_NUM = fdfs2qq::GetCpuCoreCount;

fdfs2qq::ConcurrentQueueUnique_lock<string> ProducerCli::G_ItemProduce_Mq;
fdfs2qq::concurrent_queue<std::string> ProducerCli::G_Volumns_Mq;
void ProducerCli::Init() {
	pTrackerServer = new ConnectionInfo;
	strcpy(pTrackerServer->ip_addr, fdfs2qq::TRACKER_IP().c_str());
	pTrackerServer->port = fdfs2qq::TRACKER_PORT();
}

int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data) {
	int result = tcpsenddata_nb(pCurrentServer->sock, (void*) data.c_str(),
			data.length(), g_fdfs_network_timeout);
	if (result != 0) {
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
		"sync appender file, tcpsenddata_nb: %s error, "
		"maybe disconnect? later? reason status %d :%s,",
		__LINE__, data.c_str(), result,
		STRERROR(result));
	}
}
int ProducerCli::TransferByData(ConnectionInfo* pCurrentServer,
		const string& data, RESPONSE_HEADER &ret) {
	const int g_fdfs_network_timeout = 20;

	const int connect_timeout = 20;
	int * err_no = new int;
	*err_no = conn_pool_connect_server(pCurrentServer, connect_timeout);

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


	const int g_fdfs_network_timeout = 20;
	const int connect_timeout = 20;
	int * err_no = new int;
	*err_no = conn_pool_connect_server(pCurrentServer, connect_timeout);

	while (std::getline(t, tmp)) {
		tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
		stringstream sm;
		sm << fdfs2qq::CMD::LSM << fdfs2qq::BOX_MSG_SEPERATOR << tmp << "\n";
		tmp = sm.str();
		fdfs2qq::Logger::info(tmp);
		int result = TransferByData(pCurrentServer, tmp);
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
	for (auto item : StorageConfig::VolumnsDict) {
		string tmp(item.first);
		StorageConfig::getStoreFolderByOrder(v, tmp);
	}
	//set queue
	for (std::vector<fdfs2qq::StorageVolumnObject>::iterator it = v.begin();
			it != v.end(); it++) {
		G_Volumns_Mq.push(it->to_string());
	}

}

/****
 * producer mq start
 */

//once
void ProducerCli::ForkProducer() {

	if(!G_Volumns_Mq.empty()){
		string str;
				G_Volumns_Mq.try_pop(str);
		fdfs2qq::StorageVolumnObject ori_obj=StringToStorageVolumnObject(str);
		_ProducerMsg(ori_obj);
	}

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
	while (true) {
		if(!G_Volumns_Mq.empty()){
			string str;
			G_Volumns_Mq.try_pop(str);
			fdfs2qq::StorageVolumnObject ori_obj=StringToStorageVolumnObject(str);
			_ProducerMsg(ori_obj);
		}else{
			perror("exit,for produce finished \n");
			_ProducerStopMsg();
			pthread_exit(NULL); // producer exit

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
			fdfs2qq::Logger::info(tmp);
			const size_t bodysize=sizeof(RESPONSE_HEADER);

			RESPONSE_HEADER header;
			int result = TransferByData(pTrackerServer, tmp, header);
			auto stssatus =stoi(header.ext_status);
//			std::string  status_str=std::string(header.ext_status);
			if ( stssatus== RESPONSE_STATUS::REGISTSUCESS) {
				fdfs2qq::StorageFileObject fileobj=ResponseObject2StorageFileObject(header);
				//G_ItemProduce_Mq.push(fileobj.global_fileid);

//sync
						fdfs2qq::Logger::info(fileobj.global_fileid);
						fdfs2qq::Logger::info(fileobj.physical_store_path);
						_ConsumerMsg(fileobj);




		//end
				fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"tracker server %s:%d, recv data fail, "
				"errno: %d, error info: %s, status:%s, filid :%s ",
				__LINE__, pTrackerServer->ip_addr, pTrackerServer->port, result,
						STRERROR(result), header.ext_status,header.fileid);
			} else {
				fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
							"depelated, filid :%s ",
							__LINE__, header.fileid);
			}
		} else {
			fdfs2qq::Logger::error(
					*it + ",\tfail index G_BptList,for index=" + volumns_str);
		}
	}

}


void ProducerCli::ForkConsumer() {

	int max_c = MAX_CONSUME_THREAD_NUM;

	Logger::debug("consume thread num" + std::to_string(max_c));
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
		if (!G_ItemProduce_Mq.empty()) {
			string fileid;
			G_ItemProduce_Mq.pop(fileid);
			auto str_fileid=std::string(fileid.c_str());
			const StorageFileObject msg=Strfileid2StorageFileObject(str_fileid);
			fdfs2qq::Logger::info(str_fileid);
			fdfs2qq::Logger::info(msg.physical_store_path);
			_ConsumerMsg(msg);
		} else {
//			transfer::tcp::SendByUnixDomain(item_socket_path,"d");

			pthread_cond_broadcast(& G_Condition_variable);	//wake up producer to working
		}
	}
}
void ProducerCli::_ConsumerMsg(const StorageFileObject &info) {

	if (info.physical_store_path.empty()) {
		return;
	}
	const std::string physical_fullpath = info.physical_store_path;
	const std::string physical_fullpath_meta = info.physical_store_path + "-m";

	//box build
	std::string ret;
	std::vector<std::string> input = { "overwrite_flag\t0" };
	stringstream formstr;
	formstr << "fileid\t" << info.global_fileid;
	input.push_back(formstr.str());
	formstr.str("");
	formstr << "file_meta\t" << physical_fullpath_meta ;
	input.push_back(formstr.str());
	formstr.str("");
	formstr << "filehandle\t" << physical_fullpath;
	input.push_back(formstr.str());

	input.push_back("c1\tv1");
	int size = input.size();
	stringstream scout;
	for (std::vector<std::string>::iterator it = input.begin();
			it != input.end(); it++) {
		scout << *it;
		if (it + 1 != input.end())
			scout << box::field::BOX_RECORD_SEPERATOR;
	}
	ret.assign(scout.str());
	stringstream ss;
	ss << ret << box::field::BOX_FIELD_SEPERATOR << ""
			<< box::field::BOX_MSG_SEPERATOR;
	const string origin_msg = ss.str();
	int msgcount = 0;
	std::vector<MessageItem> ret_s = SplitMsg(origin_msg.c_str(),
			origin_msg.length(), msgcount);
	//box build end

	if (ret_s.size() > 0) {
		writer::tencent::TencentStorageServiceWriter *wr =
				new writer::tencent::TencentStorageServiceWriter;
		auto rsp = wr->messageHeaderFormat(&ret_s[0])->messageBodyFormat(
				&ret_s[0])->messageOutputStream();
		wr->sprintf_response(rsp);
	}
}

}
