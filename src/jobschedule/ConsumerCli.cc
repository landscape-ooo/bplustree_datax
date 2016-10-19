/**
 * Multithreaded, libevent-based socket server.
 * Copyright (c) 2012 Ronald Bennett Cemer
 * This software is licensed under the BSD license.
 * See the accompanying LICENSE.txt for details.
 *
 * To compile: gcc -o echoserver_threaded echoserver_threaded.c workqueue.c -levent -lpthread
 * To run: ./echoserver_threaded
 */
#include "ConsumerCli.h"
namespace jobschedule{
/* Port to listen on. */
const int ConsumerCli::SERVER_PORT = fdfs2qq::CONSUME_PORT();
/* Connection backlog (# of backlogged connections to accept). */
const int ConsumerCli::CONNECTION_BACKLOG = fdfs2qq::CONNECTION_BACKLOG;
/* Socket read and write timeouts, in seconds. */
const int ConsumerCli::SOCKET_READ_TIMEOUT_SECONDS = fdfs2qq::G_FDFS_NETWORK_TIMEOUT;
const int ConsumerCli::SOCKET_WRITE_TIMEOUT_SECONDS = fdfs2qq::G_FDFS_NETWORK_TIMEOUT;
/* Number of worker threads.  Should match number of CPU cores reported in /proc/cpuinfo. */
const int ConsumerCli::NUM_THREADS = GetCpuCoreCount;







struct event_base * ConsumerCli::_Bvbase_accept;
workqueue_t ConsumerCli::_Workqueue;

///* Signal handler function (defined below). */
//void ConsumerCli::sighandler(int signal);

/**
 * Set a socket to non-blocking mode.
 */
int ConsumerCli::_Setnonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0) return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) return -1;
	return 0;
}

void ConsumerCli::_CloseClient(client_t *client) {
	if (client != NULL) {
		if (client->fd >= 0) {
			close(client->fd);
			client->fd = -1;
		}
	}
}

void ConsumerCli::_CloseAndFreeClient(client_t *client) {
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
void ConsumerCli::_Buffered_on_write(struct bufferevent *bev, void *arg) {
}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void ConsumerCli::_Buffered_on_error(struct bufferevent *bev, short what, void *arg) {
	_CloseClient((client_t *)arg);
}

void ConsumerCli::_Server_job_function(struct job *job) {
	client_t *client = (client_t *)job->user_data;

	event_base_dispatch(client->evbase);
	_CloseAndFreeClient(client);
	free(job);
}

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void ConsumerCli::_On_accept(int fd, short ev, void *arg) {
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
	if ((client_ptr->buf_ev = bufferevent_new(client_fd, &ConsumerCli::_Buffered_on_read, &ConsumerCli::_Buffered_on_write, &ConsumerCli::_Buffered_on_error, client_ptr)) == NULL) {
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
int ConsumerCli::RunServer(void) {
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
		err(1, "listen failed");
	}
	int yes=1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	if (::bind(listenfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
		err(1, "bind failed");
	}
	if (::listen(listenfd, CONNECTION_BACKLOG) < 0) {
		err(1, "listen failed");
	}
	reuseaddr_on = 1;
	::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (_Setnonblock(listenfd) < 0) {
		err(1, "failed to set server socket to non-blocking");
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
void ConsumerCli::_KillServer(void) {
	fdfs2qq::Logger::info("Stopping socket listener event loop.\n");
	if (event_base_loopexit(_Bvbase_accept, NULL)) {
		fdfs2qq::Logger::error("Error shutting down server");
	}
	fdfs2qq::Logger::info( "Stopping workers.\n");
	workqueue_shutdown(&_Workqueue);
}

void ConsumerCli::_Sighandler(int signal) {
	fdfs2qq::Logger::info( "Received signal %d: %s.  Shutting down.\n", signal, strsignal(signal));
	_KillServer();
}

/**
 * Called by libevent when there is data to read.
 */
void ConsumerCli::_Buffered_on_read(struct bufferevent *bev, void *arg) {
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
	const string fileid=std::string(&data[0]);
//	const string fileid=std::string(&data[0],nbytes_chunk_total>box::field::BUFFER_MAX_LINE?box::field::BUFFER_MAX_LINE:nbytes_chunk_total);
	fdfs2qq::Logger::info(fileid);
	_ConsumerMsg(fileid);


	/* Send the results to the client.  This actually only queues the results for sending.
	 * Sending will occur asynchronously, handled by libevent. */
	if (bufferevent_write_buffer(bev, client->output_buffer)) {
		ErrorOut("Error sending data to client on fd %d\n", client->fd);
		_CloseClient(client);
	}
}

void ConsumerCli::_ConsumerMsg(const string  &infostr) {
	StorageFileObject info_tmp;
	try{
	info_tmp=fdfs2qq::Strfileid2StorageFileObject(infostr);
	}catch(std::exception &ex){
		fdfs2qq::Logger::error(ex.what());
	}
	const StorageFileObject info(info_tmp);
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
	std::vector<box::field::MessageItem> ret_s =box::field:: SplitMsg(origin_msg.c_str(),
			origin_msg.length(), msgcount);
	//box build end

	if (ret_s.size() > 0) {
		writer::tencent::TencentStorageServiceWriter *wr =
				new writer::tencent::TencentStorageServiceWriter;
		auto rsp = wr->messageHeaderFormat(&ret_s[0])->messageBodyFormat(
				&ret_s[0])->messageOutputStream();
		wr->sprintf_response(rsp);
		delete wr;
	}
}

}






int main(int argc, const char *argv[]) {

	struct FastLogStat logstat = { kLogAll, kLogFatal, kLogSizeSplit };
	FastLog::OpenLog(fdfs2qq::LOGPREFIX().c_str(), "fdfs2qq_consume", 2048, &logstat,
			NULL);

	installSignal(SIGSEGV);

	jobschedule::ConsumerCli::RunServer();

	return 1;
}





