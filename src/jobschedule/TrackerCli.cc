/*
 * TrackerCli.cc
 *
 *  Created on: Sep 5, 2016
 *      Author: a11
 */
#include "TrackerCli.h"
namespace jobschedule {

msgInfo ParseString2MsgInfo(const string& volumnstr,
		const string& global_fileid) {
	// {volumnstr}\t{global_fileid}
	msgInfo retinfo;
	fdfs2qq::StorageFileObject obj_file(global_fileid, volumnstr);
	retinfo.FileObject = obj_file;
	retinfo.clientId = 2;
	retinfo.fileId_hash = fdfs2qq::Gethash(global_fileid);
	return retinfo;
}

::pthread_mutex_t TrackerCli::G_bplus_tree_Ptrmutex;
concurrent_queue<msgInfo> TrackerCli::QUEUE_MSG;
const int TrackerCli::SERVER_PORT = htons(fdfs2qq::TRACKER_PORT());
const int TrackerCli::MAX_GRP_ID = fdfs2qq::MAX_GRP_ID();
bpt::bplus_tree ** TrackerCli::G_BptList;

void TrackerCli::InitBpt() {
	try{
	//init memory
	G_BptList = new bpt::bplus_tree*[TrackerCli::MAX_GRP_ID];
	for (int i = 0; i < TrackerCli::MAX_GRP_ID; i++) { //init tree list
		string dbname = std::to_string(i) + "_bplus_tree.db";
		*(G_BptList + i) = new bpt::bplus_tree(dbname.c_str(), false);
	}
	}catch(std::exception &ex){
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"InitBpt function error, reason:%s "
				,__LINE__,ex.what());
	}
}
bpt::bplus_tree* TrackerCli::GetBptByMsginfo(const struct msgInfo& msg) {
	if(G_BptList!=NULL){
		auto grpid = fdfs2qq::String2int(msg.FileObject.grpid);
		return *(G_BptList + grpid);
	}
	fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
			"GetBptByMsginfo function error,null pointer recv, clientId:%s fileId:%s fileId_hash%s"
			,__LINE__,msg.clientId,msg.fileId.c_str(),msg.fileId_hash.c_str());
	return NULL;
}

void TrackerCli::StartRecvBinlog() {

	const int threadnum = fdfs2qq::GetCpuCoreCount;
	fdfs2qq::Logger::info("cpuinfo:" + std::to_string(threadnum) + "\n");
	pthread_t pool[threadnum];
	//pthread_t log_thread;
	for (int i = 0; i < threadnum; i++) {
		::pthread_create(&pool[i], NULL, &TrackerCli::_Recvbinlog, 0);
		::pthread_detach(pool[i]);
	}
}

void* TrackerCli::_Recvbinlog(void*) {
	struct msgInfo msg;
	while (true) {
		if(QUEUE_MSG.empty()){
			continue;
		}
		QUEUE_MSG.try_pop(msg);
		fdfs2qq::Logger::info("get msg:" + msg.FileObject.global_fileid + "\n");

		string fileid = msg.FileObject.global_fileid;
		std::string keys = fileid;
		if (keys.empty()){
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
					"NULL key , QUEUE_MSG size =%d "
								,__LINE__,QUEUE_MSG.size());
			continue;
		}
		bpt::bplus_tree* bptHandle = GetBptByMsginfo(msg);
		bpt::value_t* v_pt = new int;
		bpt::key_t* k_ptr = new bpt::key_t(keys.c_str());
		bool findit = false;
		auto plusptr = TrackerCli::GetBptByMsginfo(msg);
		pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
		findit = BptDelegate::SearchBptByKey(plusptr, k_ptr, v_pt);
		if (!findit) {
			fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
								" key:%s  is empty , bpt insert  %d "
										,__LINE__,keys.c_str(),	msg.clientId);
			BptDelegate::InsertAndUpdateBptfromData(plusptr, keys,
					msg.clientId);
		}
		pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);

		delete v_pt;
		delete k_ptr;
	}

}

bool TrackerCli::_BinlogHandle(const string msgstr) {
	bool flag = true;
	string volumnstr;
	string global_fileid;
	//sendfile
	if (strncmp(msgstr.c_str(), "gjfs", strlen("gjfs")) == 0) {
		//start with gjfs
		volumnstr = "";
		global_fileid = msgstr;
		flag = true;
	} else if (msgstr.find('\t') != string::npos) {
		//send str
		auto vecs = fdfs2qq::split(msgstr, '\t');
		volumnstr = vecs[0];
		global_fileid = vecs[1];
		flag = true;
	}
	return _BinlogHandle(global_fileid, volumnstr);
}

bool TrackerCli::_BinlogHandle(const string global_fileid,
		const string volumnstr) {
	if (!global_fileid.empty()) {
		auto msginfo = ParseString2MsgInfo(volumnstr, global_fileid);
		QUEUE_MSG.push(msginfo);
		return true;
	}
	return false;

}
/**
 * @return
 * -1 error
 * 1 exist
 * 2 not exist,regist
 */
int TrackerCli::_QueryHandle(const string msgstr) {
	auto msg = ParseString2MsgInfo("", msgstr);
	string fileid = msg.FileObject.global_fileid;
	if (fileid.empty()) {
		return -1;
	}
	bpt::bplus_tree* bptHandle = GetBptByMsginfo(msg);
	bpt::value_t* v_pt = new int;
	bpt::key_t* k_ptr = new bpt::key_t(fileid.c_str());
	bool findit = false;
	auto plusptr = TrackerCli::GetBptByMsginfo(msg);
	pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
	findit = BptDelegate::SearchBptByKey(plusptr, k_ptr, v_pt);
	delete k_ptr;delete v_pt;
	pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
	if (!findit) {
		fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
		"recv query req : %s error, query fail ",
		__LINE__, msgstr.c_str());
		pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
		BptDelegate::InsertAndUpdateBptfromData(plusptr, fileid, msg.clientId);
		pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
		return 2;
	} else {
		fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
		"recv query req : %s error, query success! ",
		__LINE__, msgstr.c_str());
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
	addresslisten.sin_port = (SERVER_PORT);

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
		fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
			"client establish!client ip : %d client port :%d ",
			__LINE__, client_addr.sin_addr.s_addr,client_addr.sin_port);
	}

	auto setnonblock = [](int fd) -> int //Cool!
			{
				int flags;

				flags = fcntl(fd, F_GETFL);
				flags |= O_NONBLOCK;
				fcntl(fd, F_SETFL, flags);
			}; //setnonblock is actually a variable, so don't forget the ;

	setnonblock(client_fd);
	client = new EventClient();
	if (client == NULL) {
		fdfs2qq::Logger::info("file: " __FILE__ ", line: %d, "
					"malloc failed",
					__LINE__);
	}
	client->fd = client_fd;

	client->buf_ev = bufferevent_new(client_fd, _EventReaderCallback,
			_EventWriterCallback, _EventErrorCallback, client);

	bufferevent_enable(client->buf_ev, EV_READ);
//	delete client;

}

void TrackerCli::_EventWriterCallback(struct bufferevent* bev, void* arg) {
}

void TrackerCli::_EventReaderCallback(struct bufferevent* incoming, void* arg) {
	struct evbuffer *evreturn;
	char *req;

	struct evbuffer *output;

	output = bufferevent_get_output(incoming); //其实就是取出bufferevent中的output

	req = evbuffer_readline(incoming->input);
	if (req == NULL)
		return;
	std::string msg(req);
	std::string charone, volumnstr, fileid;

	auto ves = fdfs2qq::split(msg, fdfs2qq::BOX_MSG_SEPERATOR);

	if (ves.size() == 3) {
		charone = std::string(ves[0]);
		volumnstr = std::string(ves[1]);
		fileid = std::string(ves[2]);
		msg = fileid;
	} else if (ves.size() == 2) {
		charone = std::string(ves[0]);
		volumnstr = std::string("");
		fileid = std::string(ves[1]);
		msg = fileid;
	}else{
		fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
				"query_requst_msg  parse error,req:%s",
				__LINE__,req);
	}
//	fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
//	"recv request data : %s error, "
//	"remind.....",
//	__LINE__, msg.c_str());
	evreturn = evbuffer_new();
	switch (::atoi(charone.c_str())) {
	case CMD::LSM: {
		auto flag = _BinlogHandle(fileid, volumnstr);
		evbuffer_add_printf(evreturn, flag ? "ok\n" : "wrong\n", req);
	}
		break;
	case CMD::REGIST: {
		int response ;
		try{
			response= _QueryHandle(fileid);
		}catch(std::exception &ex){
			fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
							"_QueryHandle  error,fileid:%s",
							__LINE__,fileid.c_str());
		}
		int ret;
		switch (response) {
		case 2:
			ret = RESPONSE_STATUS::REGISTSUCESS;
			break;
		case 1:
			ret = RESPONSE_STATUS::EXIST;
			break;
		default:
			ret = RESPONSE_STATUS::NOTFOUND;
			break;
		}
		RESPONSE_HEADER header;
		memset(&header, 0, sizeof(header));
		header = StringToResponseObject(fileid, volumnstr,std::to_string(ret));
		std::string rep_str = ResponseObjectToString(header);
		evbuffer_add(output, rep_str.c_str(), rep_str.length());
		fdfs2qq::Logger::info(
				"file: " __FILE__ ", line: %d, "
				"response status %d,total  send  data : %d error,total actual buffer send %d "
				"remind.....",
				__LINE__, response, rep_str.length(), rep_str.length());
	}
		break;
	default:
		evbuffer_add_printf(evreturn,
				std::to_string(RESPONSE_STATUS::NOTFOUND).c_str());
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

