/*
 * ConsumerCli.cc
 *
 *  Created on: Aug 26, 2016
 *      Author: a11
 */

#include "ConsumerCli.h"
namespace jobschedule {


int ConsumerCli::MAX_CONSUME_THREAD_NUM = fdfs2qq::GetCpuCoreCount;
const char * ConsumerCli::item_socket_path = fdfs2qq::RECV_SOCKET_PATH();
const char * ConsumerCli::_notify_socket_path = fdfs2qq::SOCKET_PATH();

//::pthread_cond_t ConsumerCli::G_Condition_variable;

void ConsumerCli::Init() {
}
void ConsumerCli::InitQueue() {
}

void ConsumerCli::ForkConsumer() {

	int max_c = MAX_CONSUME_THREAD_NUM;

	Logger::debug("consume thread num" + std::to_string(max_c));
	pthread_t pool[max_c];
	//pthread_t log_thread;
	for (int i = 0; i < max_c; i++) {
		::pthread_create(&pool[i], NULL, &ConsumerCli::ListenItemConsumerMq,
				NULL);
		::pthread_detach(pool[i]);
	}
}

void* ConsumerCli::ListenItemConsumerMq(void*) {
	while (true) {
		if (!fdfs2qq::G_ItemProduce_Mq.empty()) {
			const StorageFileObject msg(fdfs2qq::G_ItemProduce_Mq.try_pop());
			//exit char????
//			if(msg.IS_PROGRAME_THREAD_STOP==true){
//				fdfs2qq::G_ItemProduce_Mq.push(msg);//push back
//				fdfs::Logger::debug("exit,for consume finished \n");
//				pthread_exit(NULL);// producer exit
//				break;
//			}else{
			//normal consume
			_ConsumerMsg(msg);
//			}
		} else {
//			transfer::tcp::SendByUnixDomain(item_socket_path,"d");

			pthread_cond_broadcast(& fdfs2qq::G_Condition_variable);	//wake up producer to working
		}
	}
}
void ConsumerCli::_ConsumerMsg(const StorageFileObject &info) {

	if (info.physical_store_path.empty()) {
		return;
	}
	const std::string physical_fullpath = info.physical_store_path;
	const std::string physical_fullpath_meta = info.physical_store_path + "-m";

	//box build
	std::string ret;
	std::vector<std::string> input = { "overwrite_flag\t1", "c1\tv1", "d2\tv2" };
	stringstream formstr;
	formstr << "fileid\t" << info.global_fileid;
	input.push_back(formstr.str());
	formstr.str("");
	formstr << "filehandle\t" << physical_fullpath;
	input.push_back(formstr.str());
	formstr.str("");
	formstr << "file_meta\t" << physical_fullpath_meta << "-m";
	input.push_back(formstr.str());

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
//	//lsm log
//	char buffer[fdfs::MAX_BUFFER_SIZE];
//	int lens=sprintf(buffer,"%s\t%s\t%d\n",
//			msg.dir_info_ptr.volumns_id.c_str(),
//			msg.global_object_id.c_str(),
//			1);
//	string retmsg(buffer,lens);
//
//	G_ResultLogMq.push(retmsg); //callback
}

//void ConsumerCli::_ConsumerMsg(
//		const fdfs2qq::StorageVolumnObject&) {
//}

void ConsumerCli::_ConsumerStopMsg() {
}
/****
 * Consumer mq end
 */

}

