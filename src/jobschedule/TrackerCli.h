/*
 * TrackerCli.h
 *
 *  Created on: Aug 30, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_
#define SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <event.h>
#include <signal.h>


#include "ganji/ganji_global.h"

#include "common/common_define.h"
#include "common/workqueue.h"
#include "common/mq.h"
#include "common/MongoDelegate.h"
#include "common/tools.h"

#include "box/StorageConfig.h"
#include "box/box_object.h"

#include "writer/TencentStorageServiceWriter.h"
#include "transfer/unixdomain.h"
extern "C"{
	#include "fastdfs/common/shared_func.h"
}
#include "ganji/util/log/thread_fast_log.hpp"
using namespace ganji::util::log::ThreadFastLog;
namespace FastLog = ganji::util::log::ThreadFastLog;


namespace jobschedule{
using namespace std;
using namespace fdfs2qq;

typedef struct StructClient {
	/* The client's socket. */
	int fd;

	/* The event_base for this client. */
	struct event_base *evbase;

	/* The bufferedevent for this client. */
	struct bufferevent *buf_ev;

	/* The output buffer for this client. */
	struct evbuffer *output_buffer;

	/* Here you can add your own application-specific attributes which
	 * are connection-specific. */
} client_t;

/* Behaves similarly to fprintf(stderr, ...), but adds file, line, and function
 information. */
#define ErrorOut(...) {\
		fdfs2qq::Logger::error( "%s:%d: %s():\t", __FILE__, __LINE__, __FUNCTION__);\
		fdfs2qq::Logger::error( __VA_ARGS__);\
}



msgInfo ParseString2MsgInfo(const string&,const string&);

class TrackerCli {
//	static bpt::bplus_tree* treePtr;
//	static bpt::bplus_tree ** G_BptList;


	static concurrent_queue<msgInfo> QUEUE_MSG;
public:
	static void InitBpt();
	static void ForkRecvBinlogHandle() ;
//	static void RegisteEvent();

	static int RunServer(void);

private:

//	static bpt::bplus_tree* GetBptByMsginfo(const struct msgInfo& msg);


	static void* _ProcessRecvbinlog(void*) ;

	/****
	 * <-------event  start
	 */
	static bool _BinlogHandle(const string msgstr);
	static bool _BinlogHandle(const string fileid,
			const string volumnstr);
	static int _QueryHandle(const string msgstr);




	static void _KillServer(void);
	/* Signal handler function (defined below). */
	static void _Sighandler(int signal);
	static int _Setnonblock(int fd);
	static void _CloseClient(client_t *client);
	static void _CloseAndFreeClient(client_t *client);
	static void _Buffered_on_read(struct bufferevent *bev, void *arg);
	static void _On_accept(int fd, short ev, void *arg);
	static void _Server_job_function(struct job *job);
	static void _Buffered_on_error(struct bufferevent *bev, short what,
			void *arg);
	static void _Buffered_on_write(struct bufferevent *bev, void *arg);

	/****
	 * <-------event  start
	 */

private:
	static ::pthread_mutex_t G_produce_Ptrmutex;
//	static ::pthread_mutex_t G_bplus_tree_Ptrmutex;


	static const int  SERVER_PORT ;
	static const  int  MAX_GRP_ID;
	static struct event_base *_Bvbase_accept;
	static workqueue_t _Workqueue;

	static const int CONNECTION_BACKLOG;
	static const int SOCKET_READ_TIMEOUT_SECONDS;
	static const int SOCKET_WRITE_TIMEOUT_SECONDS;
	static const int NUM_THREADS;

};

}
#endif /* SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_ */
