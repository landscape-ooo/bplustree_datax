/*
 * ConsumerCli.h
 *
 *  Created on: Aug 26, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_JOBSCHEDULE_CONSUMERCLI_H_
#define SRC_SRC_JOBSCHEDULE_CONSUMERCLI_H_

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
#include <pthread.h>
#include <cctype>
#include <string>
#include <functional>
#include <algorithm>
#include "common/common_define.h"
#include "common/mq.h"
#include "common/BptDelegate.h"
#include "box/StorageConfig.h"
#include "box/box_object.h"
#include "writer/TencentStorageServiceWriter.h"

#include "transfer/unixdomain.h"
namespace jobschedule {
using namespace box::field;
using namespace std;
#ifndef SRC_SRC_EVENTCLIENT_H_
#define SRC_SRC_EVENTCLIENT_H_
struct EventClient {
  int fd;
  struct bufferevent *buf_ev;
};
#endif
class ConsumerCli {
public:
	static int MAX_Consumer_THREAD_NUM;
	static int MAX_CONSUME_THREAD_NUM;
	static void Init();
public:

	/****
	 * ------>Consumer mq start
	 */
	static void ForkConsumer();
	static void* ListenItemConsumerMq(void*);
	static void _ConsumerMsg(const fdfs2qq::StorageFileObject &);
	static void _ConsumerStopMsg();
	static const string NULL_MSG_SIGNAL;
	/****
	 * <-------Consumer mq end
	 */

	/****
	 * <-------event  start
	 */
	static const char * item_socket_path;
	static const char * _notify_socket_path ;
	static void InitEvent();
	static void RegisteEvent();
	static void _EventAcceptCallback(int fd,
            short ev,
            void *arg);
	static void _EventWriterCallback(struct bufferevent *bev,
            void *arg);
	static void _EventReaderCallback(struct bufferevent *incoming,
            void *arg);
	static void _EventErrorCallback(struct bufferevent *bev,
            short what,
            void *arg);

	/****
	 * <-------event  start
	 */
private:
	static void _SendUnixDomain(const string&);
	static void InitQueue();

//	static fdfs2qq::concurrent_queue<fdfs2qq::StorageFileObject> G_Item_Mq;

//	static ::pthread_mutex_t G_produce_Ptrmutex;
//	static ::pthread_mutex_t G_bplus_tree_Ptrmutex;

	static ::pthread_cond_t G_Condition_variable;

//	static bpt::bplus_tree ** G_BptList;

	//static fdfs2qq::Sender* SenderPtr;

//	static fdfs2qq::concurrent_queue<string> G_ResultLogMq;

//	static fdfs2qq::concurrent_queue<fdfs2qq::StorageVolumnObject> G_Volumns_Mq;


};
}



#endif /* SRC_SRC_JOBSCHEDULE_CONSUMERCLI_H_ */
