/*
 * CliProducer.h
 *
 *  Created on: Aug 25, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_JOBSCHEDULE_CLIPRODUCER_H_
#define SRC_SRC_JOBSCHEDULE_CLIPRODUCER_H_
////event

#include <event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

///event end
#include <queue>

#include <pthread.h>
#include <cctype>
#include <string>
#include <functional>
#include <algorithm>


#include "ganji/ganji_global.h"
#include "common/common_define.h"
//#include "common/concurrentqueue.h"
#include "common/BptDelegate.h"
#include "box/StorageConfig.h"
#include "transfer/tcp_transfer.h"
#include "transfer/unixdomain.h"

#include "box/box_object.h"
#include "writer/TencentStorageServiceWriter.h"


#include "ganji/util/log/thread_fast_log.hpp"
using namespace ganji::util::log::ThreadFastLog;
namespace FastLog = ganji::util::log::ThreadFastLog;



namespace jobschedule {
using namespace std;
using namespace box::field;
#ifndef SRC_SRC_EVENTCLIENT_H_
#define SRC_SRC_EVENTCLIENT_H_
struct EventClient {
  int fd;
  struct bufferevent *buf_ev;
};
#endif
class ProducerCli {
private:
	static ::pthread_cond_t G_Condition_variable;
	static int MAX_PRODUCER_THREAD_NUM;
	static int MAX_CONSUME_THREAD_NUM;
	static const char * _notify_socket_path;
	static const char * item_socket_path;
public:
	/****
	 * ------>producer mq start
	 */
	static void Init();
	static void ForkProducer();
	static void ForkConsumer();
	static void* WaitProducer(void*);
	static void LSMBinlog();
	static void ReadyProducer();
	/****
	 * <-------producer mq end
	 */
	/**
	 * event
	 */
	static void RegisteEvent();
	/**
		 * event end
		 */
private:
	static void* ListenItemConsumerMq(void*);
	static void _ConsumerMsg(const StorageFileObject &info) ;
	static void _EventSignalCallback(evutil_socket_t fd, short event, void *arg);

	static void _SendUnixDomain(const RESPONSE_HEADER& resp);
	static void _EventAcceptCallback(int fd, short ev, void* arg);
	static void _EventWriterCallback(struct bufferevent* bev, void* arg) ;
	static void _EventReaderCallback(struct bufferevent* incoming,
			void* arg);
	static void _EventErrorCallback(struct bufferevent* bev, short what,
			void* arg) ;


	static void _ProducerMsg(const fdfs2qq::StorageVolumnObject &);
	static void _ProducerStopMsg();
	static const string NULL_MSG_SIGNAL;
	static void TransferByFilename(ConnectionInfo* ,const string );
	static int TransferByData(ConnectionInfo* pCurrentServer,\
			const string& data,RESPONSE_HEADER &);
	static int TransferByData(ConnectionInfo* pCurrentServer,\
				const string& data);



	static ConnectionInfo* pTrackerServer;
	static ConnectionInfo* pConsumerServer;



	static fdfs2qq::concurrent_queue<string> G_ItemProduce_Mq;
	static fdfs2qq::concurrent_queue<StorageVolumnObject> G_Volumns_Mq;

	static ::pthread_mutex_t G_produce_Ptrmutex;
	static ::pthread_mutex_t G_bplus_tree_Ptrmutex;


	static bpt::bplus_tree ** G_BptList;




};
}
#endif /* SRC_SRC_JOBSCHEDULE_CLIPRODUCER_H_ */
