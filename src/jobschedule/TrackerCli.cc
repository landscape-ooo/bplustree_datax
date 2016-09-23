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
const int TrackerCli::SERVER_PORT = (fdfs2qq::TRACKER_PORT());
const int TrackerCli::MAX_GRP_ID = fdfs2qq::MAX_GRP_ID();
bpt::bplus_tree ** TrackerCli::G_BptList;




/* Port to listen on. */
/* Connection backlog (# of backlogged connections to accept). */
const int TrackerCli::CONNECTION_BACKLOG = fdfs2qq::CONNECTION_BACKLOG;
/* Socket read and write timeouts, in seconds. */
const int TrackerCli::SOCKET_READ_TIMEOUT_SECONDS = fdfs2qq::G_FDFS_NETWORK_TIMEOUT;
const int TrackerCli::SOCKET_WRITE_TIMEOUT_SECONDS = fdfs2qq::G_FDFS_NETWORK_TIMEOUT;
/* Number of worker threads.  Should match number of CPU cores reported in /proc/cpuinfo. */
const int TrackerCli::NUM_THREADS = GetCpuCoreCount;







struct event_base * TrackerCli::_Bvbase_accept;
workqueue_t TrackerCli::_Workqueue;


void TrackerCli::InitBpt() {
	try{
	//init memory
	G_BptList = new bpt::bplus_tree*[TrackerCli::MAX_GRP_ID];
	for (int i = 0; i < TrackerCli::MAX_GRP_ID; i++) { //init tree list
		string dbname = fdfs2qq::to_string(i) + "_bplus_tree.db";
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

void TrackerCli::ForkRecvBinlogHandle() {

	const int threadnum = fdfs2qq::GetCpuCoreCount;
	fdfs2qq::Logger::info("cpuinfo:" + fdfs2qq::to_string(threadnum) + "\n");
	pthread_t pool[threadnum];
	//pthread_t log_thread;
	for (int i = 0; i < threadnum; i++) {
		::pthread_create(&pool[i], NULL, &TrackerCli::_ProcessRecvbinlog, 0);
		::pthread_detach(pool[i]);
	}
}

void* TrackerCli::_ProcessRecvbinlog(void*) {
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
					"NULL key , QUEUE_MSG is empty "
								,__LINE__);
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


/**
 * Set a socket to non-blocking mode.
 */
int TrackerCli::_Setnonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0) return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) return -1;
	return 0;
}

void TrackerCli::_CloseClient(client_t *client) {
	if (client != NULL) {
		if (client->fd >= 0) {
			close(client->fd);
			client->fd = -1;
		}
	}
}

void TrackerCli::_CloseAndFreeClient(client_t *client) {
	if (client != NULL) {
		_CloseClient(client);
		if (client->buf_ev != NULL) {
			bufferevent_free(client->buf_ev);
			client->buf_ev = NULL;
		}
		if (client->evbase != NULL) {
			event_base_free(client->evbase);
			client->evbase = NULL;
		}
		if (client->output_buffer != NULL) {
			evbuffer_free(client->output_buffer);
			client->output_buffer = NULL;
		}
		free(client);
	}
}









/**
 * Called by libevent when the write buffer reaches 0.  We only
 * provide this because libevent expects it, but we don't use it.
 */
void TrackerCli::_Buffered_on_write(struct bufferevent *bev, void *arg) {
}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void TrackerCli::_Buffered_on_error(struct bufferevent *bev, short what, void *arg) {
	_CloseClient((client_t *)arg);
}

void TrackerCli::_Server_job_function(struct job *job) {
	client_t *client = (client_t *)job->user_data;

	event_base_dispatch(client->evbase);
	_CloseAndFreeClient(client);
	free(job);
}

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void TrackerCli::_On_accept(int fd, short ev, void *arg) {
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	workqueue_t *workqueue = (workqueue_t *)arg;
	client_t *client_ptr;
	job_t *job;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		fdfs2qq::Logger::info("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (_Setnonblock(client_fd) < 0) {
		fdfs2qq::Logger::info("failed to set client socket to non-blocking");
		close(client_fd);
		return;
	}

	/* Create a client object. */
	//if ((client = malloc(sizeof(*client))) == NULL) {
	client_ptr= new client_t();
	if (client_ptr == NULL) {
		fdfs2qq::Logger::info("failed to allocate memory for client state");
		close(client_fd);
		return;
	}
	memset(client_ptr, 0, sizeof(*client_ptr));
	client_ptr->fd = client_fd;

	/* Add any custom code anywhere from here to the end of this function
	 * to initialize your application-specific attributes in the client struct. */

	if ((client_ptr->output_buffer = evbuffer_new()) == NULL) {
		fdfs2qq::Logger::info("client output buffer allocation failed");
		_CloseAndFreeClient(client_ptr);
		return;
	}

	if ((client_ptr->evbase = event_base_new()) == NULL) {
		fdfs2qq::Logger::info("client event_base creation failed");
		_CloseAndFreeClient(client_ptr);
		return;
	}

	/* Create the buffered event.
	 *
	 * The first argument is the file descriptor that will trigger
	 * the events, in this case the clients socket.
	 *
	 * The second argument is the callback that will be called
	 * when data has been read from the socket and is available to
	 * the application.
	 *
	 * The third argument is a callback to a function that will be
	 * called when the write buffer has reached a low watermark.
	 * That usually means that when the write buffer is 0 length,
	 * this callback will be called.  It must be defined, but you
	 * don't actually have to do anything in this callback.
	 *
	 * The fourth argument is a callback that will be called when
	 * there is a socket error.  This is where you will detect
	 * that the client disconnected or other socket errors.
	 *
	 * The fifth and final argument is to store an argument in
	 * that will be passed to the callbacks.  We store the client
	 * object here.
	 */
	if ((client_ptr->buf_ev = bufferevent_new(client_fd, &TrackerCli::_Buffered_on_read, &TrackerCli::_Buffered_on_write, &TrackerCli::_Buffered_on_error, client_ptr)) == NULL) {
		fdfs2qq::Logger::info("client bufferevent creation failed");
		_CloseAndFreeClient(client_ptr);
		return;
	}
	bufferevent_base_set(client_ptr->evbase, client_ptr->buf_ev);

	bufferevent_settimeout(client_ptr->buf_ev, SOCKET_READ_TIMEOUT_SECONDS, SOCKET_WRITE_TIMEOUT_SECONDS);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client_ptr->buf_ev, EV_READ);

	/* Create a job object and add it to the work queue. */
	//if ((job = malloc(sizeof(*job))) == NULL) {
	job =new job_t;
	if (job == NULL) {
		fdfs2qq::Logger::info("failed to allocate memory for job state");
		_CloseAndFreeClient(client_ptr);
		return;
	}
	job->job_function = _Server_job_function;
	job->user_data = client_ptr;

	workqueue_add_job(workqueue, job);
}

/**
 * Run the server.  This function blocks, only returning when the server has terminated.
 */
int TrackerCli::RunServer(void) {
	int listenfd;
	struct sockaddr_in listen_addr;
	struct event ev_accept;
	int reuseaddr_on;

	/* Initialize libevent. */
	event_init();

	/* Set signal handlers */
	sigset_t sigset;
	sigemptyset(&sigset);
//	struct sigaction siginfo = {
//		.sa_handler = sighandler,
//		.sa_mask = sigset,
//		.sa_flags = SA_RESTART,
//	};
	struct sigaction siginfo = {
		 _Sighandler,
		sigset,
		 SA_RESTART,
	};

	sigaction(SIGINT, &siginfo, NULL);
	sigaction(SIGTERM, &siginfo, NULL);

	/* Create our listening socket. */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		fdfs2qq::Logger::error("listen failed");
	}
	int yes=1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	if (::bind(listenfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
		fdfs2qq::Logger::error("bind failed");
	}
	if (::listen(listenfd, CONNECTION_BACKLOG) < 0) {
		fdfs2qq::Logger::error("listen failed");
	}
	reuseaddr_on = 1;
	::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (_Setnonblock(listenfd) < 0) {
		fdfs2qq::Logger::error("failed to set server socket to non-blocking");
	}

	if ((_Bvbase_accept = event_base_new()) == NULL) {
		fdfs2qq::Logger::error("Unable to create socket accept event base");
		close(listenfd);
		return 1;
	}

	/* Initialize work queue. */
	if (workqueue_init(&_Workqueue, NUM_THREADS)) {
		fdfs2qq::Logger::error("Failed to create work queue");
		close(listenfd);
		workqueue_shutdown(&_Workqueue);
		return 1;
	}

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
	event_set(&ev_accept, listenfd, EV_READ|EV_PERSIST, _On_accept, (void *)&_Workqueue);
	event_base_set(_Bvbase_accept, &ev_accept);
	event_add(&ev_accept, NULL);

	printf("Server running.\n");

	/* Start the event loop. */
	event_base_dispatch(_Bvbase_accept);

	event_base_free(_Bvbase_accept);
	_Bvbase_accept = NULL;

	close(listenfd);

	printf("Server shutdown.\n");

	return 0;
}

/**
 * Kill the server.  This function can be called from another thread to kill the
 * server, causing runServer() to return.
 */
void TrackerCli::_KillServer(void) {
	fdfs2qq::Logger::info( "Stopping socket listener event loop.\n");
	if (event_base_loopexit(_Bvbase_accept, NULL)) {
		fdfs2qq::Logger::error("Error shutting down server");
	}
	fdfs2qq::Logger::info( "Stopping workers.\n");
	workqueue_shutdown(&_Workqueue);
}

void TrackerCli::_Sighandler(int signal) {
	fdfs2qq::Logger::info( "Received signal %d: %s.  Shutting down.\n", signal, strsignal(signal));
	_KillServer();
}

/**
 * Called by libevent when there is data to read.
 */
void TrackerCli::_Buffered_on_read(struct bufferevent *bev, void *arg) {
	client_t *client = (client_t *)arg;
	char data[box::field::BUFFER_MAX_LINE];
	int nbytes;
	int nbytes_chunk_total=0;
	/* Copy the data from the input buffer to the output buffer in 4096-byte chunks.
	 * There is a one-liner to do the whole thing in one shot, but the purpose of this server
	 * is to show actual real-world reading and writing of the input and output buffers,
	 * so we won't take that shortcut here. */
	while ((nbytes = EVBUFFER_LENGTH(bev->input)) > 0) {
		nbytes_chunk_total+=nbytes;
		/* Remove a chunk of data from the input buffer, copying it into our local array (data). */
		if (nbytes > 4096) nbytes = 4096;
		evbuffer_remove(bev->input, data, nbytes);
		/* Add the chunk of data from our local array (data) to the client's output buffer. */
		evbuffer_add(client->output_buffer, data, nbytes);
	}


	trim_right(&data[0]);
	const string req=std::string(&data[0]);
	string msg(req);

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
					__LINE__,req.c_str());
		}
	//	fdfs2qq::Logger::error("file: " __FILE__ ", line: %d, "
	//	"recv request data : %s error, "
	//	"remind.....",
	//	__LINE__, msg.c_str());
		switch (::atoi(charone.c_str())) {
		case CMD::LSM: {
			auto flag = _BinlogHandle(fileid, volumnstr);
			evbuffer_add_printf(client->output_buffer, flag ? "ok\n" : "wrong\n", req.c_str());
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
			header = StringToResponseObject(fileid, volumnstr,fdfs2qq::to_string(ret));
			std::string rep_str = ResponseObjectToString(header);
			evbuffer_add(client->output_buffer, rep_str.c_str(), rep_str.length());
			fdfs2qq::Logger::info(
					"file: " __FILE__ ", line: %d, "
					"response status %d,total  send  data : %d error,total actual buffer send %d "
					"remind.....",
					__LINE__, response, rep_str.length(), rep_str.length());
		}
			break;
		default:
			evbuffer_add_printf(client->output_buffer,
					fdfs2qq::to_string(RESPONSE_STATUS::NOTFOUND).c_str());
			break;

		}



	/* Send the results to the client.  This actually only queues the results for sending.
	 * Sending will occur asynchronously, handled by libevent. */
	if (bufferevent_write_buffer(bev, client->output_buffer)) {
		ErrorOut("Error sending data to client on fd %d\n", client->fd);
		_CloseClient(client);
	}
}




/*******
 * event  end
 *******/

}







int main(int argc, const char *argv[]) {
	struct FastLogStat logstat = { kLogAll, kLogFatal, kLogSizeSplit };
	FastLog::OpenLog(fdfs2qq::LOGPREFIX().c_str(), "fdfs2qq_tracker", 2048, &logstat,
			NULL);

	jobschedule::TrackerCli::InitBpt();
	jobschedule::TrackerCli::ForkRecvBinlogHandle();
	jobschedule::TrackerCli::RunServer();


	return 1;
}

