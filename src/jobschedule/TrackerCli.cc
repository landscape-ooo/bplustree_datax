/*
 * TrackerCli.cc
 *
 *  Created on: Sep 5, 2016
 *      Author: a11
 */
#include "TrackerCli.h"
namespace jobschedule {

msgInfo ParseString2MsgInfo(const string& volumnstr,const string& global_fileid){
	// {volumnstr}\t{global_fileid}
	msgInfo retinfo;
	fdfs2qq::StorageFileObject obj_file(global_fileid,volumnstr);
	retinfo.FileObject=obj_file;
	retinfo.clientId=2;
	retinfo.fileId_hash=fdfs2qq::Gethash(global_fileid);
	return retinfo;
}





::pthread_mutex_t TrackerCli::G_bplus_tree_Ptrmutex;
concurrent_queue<msgInfo> TrackerCli::QUEUE_MSG;
const int  TrackerCli::SERVER_PORT=fdfs2qq::TRACKER_PORT();
const int TrackerCli::MAX_GRP_ID=fdfs2qq::MAX_GRP_ID();
bpt::bplus_tree ** TrackerCli::G_BptList;

void TrackerCli::InitBpt() {
	//init memory
	G_BptList = new bpt::bplus_tree*[TrackerCli::MAX_GRP_ID];
	for (int i = 0; i < TrackerCli::MAX_GRP_ID; i++) { //init tree list
		string dbname = std::to_string(i) + "_bplus_tree.db";
		*(G_BptList + i) = new bpt::bplus_tree(dbname.c_str(), false);
	}
}
bpt::bplus_tree* TrackerCli::GetBptByMsginfo(const struct msgInfo& msg) {
	auto grpid=fdfs2qq::String2int( msg.FileObject.grpid);
	return *(G_BptList + grpid);
}


void TrackerCli::StartRecvBinlog() {

	const int threadnum=fdfs2qq::GetCpuCoreCount;
	fdfs2qq::Logger::info("cpuinfo:"+std::to_string(threadnum)+"\n");
	pthread_t pool[threadnum];
	//pthread_t log_thread;
	for (int i = 0; i < threadnum; i++) {
		::pthread_create(&pool[i], NULL, &TrackerCli::_Recvbinlog, 0);
		::pthread_detach(pool[i]);
	}
}

void* TrackerCli::_Recvbinlog(void*) {
	struct msgInfo msg;
	while(true){
		auto flg=QUEUE_MSG.try_pop(msg);
		if(!flg) continue;
		fdfs2qq::Logger::info("get msg:"+msg.FileObject.global_fileid+"\n");

		string fileid=msg.FileObject.global_fileid;
		//std::string keys=std::to_string(fdfs2qq::Gethash(fileid));
		std::string keys=fileid;
		if(keys.empty())continue;
		bpt::bplus_tree* bptHandle=GetBptByMsginfo(msg);
		bpt::value_t* v_pt = new int;
		bpt::key_t* k_ptr = new bpt::key_t(keys.c_str());
		bool findit = false;
		auto plusptr=TrackerCli::GetBptByMsginfo(msg);
		pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
		findit = BptDelegate::SearchBptByKey(plusptr,
				k_ptr, v_pt);
		if(!findit){
			BptDelegate::InsertAndUpdateBptfromData(plusptr,
					keys,msg.clientId);
		}
		pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);

		delete v_pt;delete k_ptr;
	}

}



bool TrackerCli::_BinlogHandle(const string msgstr) {
		bool flag=true;
		string volumnstr;
		string global_fileid;
		//sendfile
		if(strncmp(msgstr.c_str(),"gjfs",strlen("gjfs"))==0){
			//start with gjfs
			volumnstr="";
			global_fileid=msgstr;
			flag=true;
		}else if (msgstr.find('\t')!=string::npos){
			//send str
			auto vecs=fdfs2qq::split(msgstr,'\t');
			volumnstr=vecs[0];
			global_fileid=vecs[1];
			flag=true;
		}
		if(flag){
			auto msginfo=ParseString2MsgInfo(volumnstr,global_fileid);
			QUEUE_MSG.push(msginfo);
		}
		return flag;
}
/**
 * @return
 * -1 error
 * 1 exist
 * 2 not exist,regist
 */
int TrackerCli::_QueryHandle(const string msgstr) {
	auto msg=ParseString2MsgInfo("",msgstr);
	string fileid=msg.FileObject.global_fileid;
	if(fileid.empty()){
		return -1;
	}
	bpt::bplus_tree* bptHandle=GetBptByMsginfo(msg);
	bpt::value_t* v_pt = new int;
	bpt::key_t* k_ptr = new bpt::key_t(fileid.c_str());
	bool findit = false;
	auto plusptr=TrackerCli::GetBptByMsginfo(msg);
	pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
	findit = BptDelegate::SearchBptByKey(plusptr,
			k_ptr, v_pt);
	pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
	if(!findit){
		pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
		BptDelegate::InsertAndUpdateBptfromData(plusptr,
				fileid,msg.clientId);
		pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
		return 2;
	}else{
		return 1;
	}

}
/******
 * event start
 ******/

void TrackerCli::RegisteEvent() {
	int socketlisten;
	struct sockaddr_in addresslisten;
	struct event accept_event;
	int reuse = 1;

	event_init();

	socketlisten = socket(AF_INET, SOCK_STREAM, 0);

	if (socketlisten < 0) {
		fprintf(stderr, "Failed to create listen socket");
		throw std::runtime_error("Failed to create listen socket");
	}
	int yes = 1;
	setsockopt(socketlisten, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&addresslisten, 0, sizeof(addresslisten));
	addresslisten.sin_family = AF_INET;
	addresslisten.sin_addr.s_addr = INADDR_ANY;
	addresslisten.sin_port=SERVER_PORT;

	if (bind(socketlisten, (struct sockaddr *) &addresslisten,
			sizeof(addresslisten)) < 0) {
		fprintf(stderr, "Failed to bind");
		throw std::runtime_error("Failed to bind");
	}

	if (listen(socketlisten, 5) < 0) {
		fprintf(stderr, "Failed to listen to socket");
		throw std::runtime_error("Failed to listen to socket");
	}

	auto setnonblock = [](int fd) -> int //Cool!
			{
				int flags;

				flags = fcntl(fd, F_GETFL);
				flags |= O_NONBLOCK;
				fcntl(fd, F_SETFL, flags);
			}; //setnonblock is actually a variable, so don't forget the ;

	setnonblock(socketlisten);

	event_set(&accept_event, socketlisten, EV_READ | EV_PERSIST,
			_EventAcceptCallback,
			NULL);

	event_add(&accept_event,
	NULL);

	event_dispatch();

	close(socketlisten);

	return;
}

void TrackerCli::_EventAcceptCallback(int fd, short ev, void* arg) {

	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct EventClient *client;

	client_fd = accept(fd, (struct sockaddr *) &client_addr, &client_len);
	if (client_fd < 0) {
		Logger::error("Client: accept() failed");
		throw std::runtime_error("Client: accept() failed");
	} else {
		printf("client establish!");
		fdfs2qq::Logger::info("client establish!");
	}

	auto setnonblock = [](int fd) -> int //Cool!
			{
				int flags;

				flags = fcntl(fd, F_GETFL);
				flags |= O_NONBLOCK;
				fcntl(fd, F_SETFL, flags);
			}; //setnonblock is actually a variable, so don't forget the ;

	setnonblock(client_fd);
	client=new EventClient();
	if (client == NULL){
		Logger::error("malloc failed");
	}
	client->fd = client_fd;

	client->buf_ev = bufferevent_new(client_fd, _EventReaderCallback,
			_EventWriterCallback, _EventErrorCallback, client);

	bufferevent_enable(client->buf_ev, EV_READ);

}

void TrackerCli::_EventWriterCallback(struct bufferevent* bev, void* arg) {
}

void TrackerCli::_EventReaderCallback(struct bufferevent* incoming,
		void* arg) {
	struct evbuffer *evreturn;
	char *req;

	req = evbuffer_readline(incoming->input);
	if (req == NULL)
		return;
	std::string  msg(req);
	std::string charone,volumnstr,fileid;

	auto ves=fdfs2qq::split(msg,fdfs2qq::BOX_MSG_SEPERATOR);

	if (ves.size()==3){
		charone=std::string(ves[0]);
		volumnstr=std::string(ves[1]);
		fileid=std::string(ves[2]);
		msg=fileid;
	}
	evreturn = evbuffer_new();
	switch(::atoi(charone.c_str())){
		case CMD::LSM:
		{
			auto flag=_BinlogHandle(msg);
			evbuffer_add_printf(evreturn,flag? "ok\n": "wrong\n", req);
		}
		break;
		case CMD::REGIST:
		{
			auto response=_QueryHandle(msg);
			string ret;
			switch(response){
				case 2:
					ret=RESPONSE_STATUS::NEWONE;
				break;
				case 1:
					ret=RESPONSE_STATUS::EXIST;
				break;
				default:
					ret=RESPONSE_STATUS::NOTFOUND;
				break;
			}
			stringstream sm;
			sm<<ret<<fdfs2qq::BOX_MSG_SEPERATOR
					<<volumnstr<<fdfs2qq::BOX_MSG_SEPERATOR
					<<fileid<<"\n";
			evbuffer_add_printf(evreturn,sm.str().c_str());
		}
		break;
		default:
			evbuffer_add_printf(evreturn,std::to_string(RESPONSE_STATUS::NOTFOUND).c_str());
		break;

	}

	bufferevent_write_buffer(incoming, evreturn);
	evbuffer_free(evreturn);
	free(req);
}


void TrackerCli::_EventErrorCallback(struct bufferevent* bev, short what,
		void* arg) {
	struct EventClient *client = (struct EventClient *) arg;
	bufferevent_free(client->buf_ev);
	close(client->fd);
	free(client);
}
/*******
 * event  end
 *******/

}



