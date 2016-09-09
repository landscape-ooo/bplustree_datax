/*
 * TrackerCli.h
 *
 *  Created on: Aug 30, 2016
 *      Author: a11
 */

#ifndef SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_
#define SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_
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
#include "common/tools.h"
namespace jobschedule {
using namespace  std;
using namespace  fdfs2qq;


msgInfo ParseString2MsgInfo(const string&,const string&);

class TrackerCli {
	static bpt::bplus_tree* treePtr;
	static bpt::bplus_tree ** G_BptList;


	static concurrent_queue<msgInfo> QUEUE_MSG;
public:
	static const int  SERVER_PORT ;
	static const  int  MAX_GRP_ID;
	static void InitBpt();
	static void StartRecvBinlog() ;
	static void RegisteEvent();
	static bpt::bplus_tree* GetBptByMsginfo(const struct msgInfo& msg);



private:



	static void* _Recvbinlog(void*) ;

	/****
	 * <-------event  start
	 */
	static bool _BinlogHandle(const string msgstr);
	static int _QueryHandle(const string msgstr);

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
	static ::pthread_mutex_t G_produce_Ptrmutex;
	static ::pthread_mutex_t G_bplus_tree_Ptrmutex;


};

}
#endif /* SRC_SRC_JOBSCHEDULE_TRACKERCLI_H_ */
