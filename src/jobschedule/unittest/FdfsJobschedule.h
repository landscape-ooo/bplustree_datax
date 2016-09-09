/*
 * FdfsProducerConsole.h
 *
 *  Created on: Aug 8, 2016
 *      Author: a11
 */

#ifndef SRC_JOBSCHEDULE_FDFSJOBSCHEDULE_H_
#define SRC_JOBSCHEDULE_FDFSJOBSCHEDULE_H_
#include <pthread.h>
#include <cctype>
#include <string>
#include <functional>
#include <algorithm>

#include "BptDelegate.h"
#include "fdfs/FdfsConf.h"
#include "fdfs/FdfsStoragMap.h"
#include "fdfs/FdfsSender.h"

namespace jobschedule{
using namespace fdfs;
class ProducerCli{
public :
	static int MAX_PRODUCER_THREAD_NUM;
	static int MAX_CONSUME_THREAD_NUM;
	static void Init();
public :

	/****
	 * ------>producer mq start
	 */
	static void ForkProducer() ;
	static void* WaitProducer(void*) ;
	static void _ProducerMsg(const fdfs::OBJECT_ID_4SUBDIR &);
	static void _ProducerStopMsg();
	/****
	 * <-------producer mq end
	 */

	/****
	 * ------------>consume item
	 */
	static void ForkItemConsumerMqHandle();
	static void* ListenItemConsumerMq(void*);
	static void _ConsumeItem(const fdfs::OBJECT_ID_4FILE &msg);
	static void _ConsumeStopMsg();
	/****
	 * <------------consume item end
	 */

	/****
	 * result mq start
	 */
	static void ForkResultMqHandle();

	static void* ListenResultMqByThread(void*) ;


	static const string NULL_MSG_SIGNAL;

private:
	static void LSMBinlog();
	static void ReadyProducer();
	static void InitConsumerHandle();

	static ::pthread_mutex_t G_produce_Ptrmutex;
	static ::pthread_mutex_t G_bplus_tree_Ptrmutex;

	static ::pthread_cond_t G_Condition_variable;

	static bpt::bplus_tree ** G_BptList;

	static fdfs::Sender* SenderPtr;

	static fdfs::concurrent_queue<string> G_ResultLogMq;

	static fdfs::concurrent_queue<fdfs::OBJECT_ID_4FILE> G_ItemProduce_Mq;
	static fdfs::concurrent_queue<fdfs::OBJECT_ID_4SUBDIR> G_Volumns_Mq;
};



}




#endif /* SRC_JOBSCHEDULE_FDFSJOBSCHEDULE_H_ */
