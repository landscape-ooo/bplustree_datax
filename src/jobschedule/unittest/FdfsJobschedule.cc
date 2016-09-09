/*
 * FdfsProducerConsole.cc
 *
 *  Created on: Aug 8, 2016
 *      Author: a11
 */
#include "FdfsJobschedule.h"
namespace jobschedule {
const string ProducerCli::NULL_MSG_SIGNAL="NULL MQ,NEED PUT MSG";
::pthread_mutex_t ProducerCli::G_bplus_tree_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;
::pthread_mutex_t ProducerCli::G_produce_Ptrmutex = PTHREAD_MUTEX_INITIALIZER;

::pthread_cond_t ProducerCli::G_Condition_variable;
bpt::bplus_tree ** ProducerCli::G_BptList;

int ProducerCli::MAX_PRODUCER_THREAD_NUM=fdfs::GetCpuCoreCount;
int ProducerCli::MAX_CONSUME_THREAD_NUM=fdfs::GetCpuCoreCount;


fdfs::concurrent_queue<string> ProducerCli::G_ResultLogMq;

fdfs::concurrent_queue<fdfs::OBJECT_ID_4FILE> ProducerCli::G_ItemProduce_Mq;
fdfs::concurrent_queue<fdfs::OBJECT_ID_4SUBDIR> ProducerCli::G_Volumns_Mq;


void ProducerCli::LSMBinlog() {

	//init memory
	G_BptList=new bpt::bplus_tree*[fdfs::StoragMap::MAX_VOLUMNS_COUNT];
	for(int i=0;i<fdfs::StoragMap::MAX_VOLUMNS_COUNT;i++){ //init tree list
		string dbname=std::to_string(i)+"_bplus_tree.db";
		*(G_BptList+i)=new bpt::bplus_tree(dbname.c_str(),false);
	}

	bpt::bplus_tree* treePtr=*(G_BptList);//first

	//simulat all filepath

	fdfs::Logger::debug("start cal keylist");
	map<string, std::vector<std::string> > ret;
	Fdfs2qqHelper::GetStoreprefix(fdfs::LOGPREFIX, fdfs::StoragMap::GROUP_ID,
			fdfs::StoragMap::MAX_VOLUMNS_COUNT,ret) ;
	//foreach key ,init a tree
	stringstream out;
	out<<"ret.size()"<<ret.size()<<"\n";
	fdfs::Logger::info(out.str());

	int index = 0;
	int index_map=0;
	for (std::map<string, std::vector<std::string> >::iterator i =
			ret.begin(); i != ret.end(); ++i) {
		treePtr=*(G_BptList+index_map);// pointer to current tree
		for(std::vector<std::string>::iterator i_list=i->second.begin();
				i_list!=i->second.end();
				i_list++
				){
			std::string fptr(*i_list);
			int value = 1; //default success
			if (fptr.find("success") == string::npos) {
				value = -1; //error
			}

			if(BptDelegate::InitBptFromFilesource(treePtr,fptr,value)>0){
				index++;
			}
		}

		index_map++;
	}
	char buffer[100];
	string formater="build finish ...total valid filecout=%d ";
	int n_len=sprintf(buffer,formater.c_str(),index);
	fdfs::Logger::info(std::string(buffer,n_len));
}

void ProducerCli::ReadyProducer(){
	fdfs::StoragMap * objptr = new fdfs::StoragMap();
	std::vector<fdfs::OBJECT_ID_4SUBDIR> v;
	objptr->getStoreFolderByOrder(v);
	//set queue
	for (std::vector<fdfs::OBJECT_ID_4SUBDIR>::iterator it = v.begin();
			it != v.end(); it++) {
		G_Volumns_Mq.push(*it);
	}

	delete objptr;
}



void ProducerCli::InitConsumerHandle(){
	SenderPtr = new fdfs::Sender(FdfsConf::serverIp);

}

void ProducerCli::Init() {
	LSMBinlog();
	ReadyProducer();
	InitConsumerHandle();
	//build bpt
}

/****
 * producer mq start
 */

//once
void ProducerCli::ForkProducer() {

//	//single
//	pthread_t produce_t;
//	fdfs::Thread_Info msg_produce;
//	::pthread_create(&produce_t,NULL,&ProducerCli::WaitProducer,0);
//	::pthread_detach(produce_t);

	//pool
	int max_c=MAX_PRODUCER_THREAD_NUM;
	pthread_t pool[max_c];
	fdfs::Thread_Info msg[max_c];
	for (int i = 0; i < max_c; i++) {
		msg[i].thread_id = i;
		msg[i].thread_msg = "w";
		::pthread_create(&pool[i], NULL, &ProducerCli::WaitProducer, &msg[i]);
		::pthread_detach(pool[i]);
	}

}


void* ProducerCli::WaitProducer(void*) {
	while(true){
		pthread_mutex_lock(&G_produce_Ptrmutex);
		pthread_cond_wait(&G_Condition_variable, &G_produce_Ptrmutex);
		_ProducerMsg(G_Volumns_Mq.try_pop());
		pthread_mutex_unlock(&G_produce_Ptrmutex);

		if(G_Volumns_Mq.empty()){
			perror("exit,for produce finished \n");
			_ProducerStopMsg();
			pthread_exit(NULL);// producer exit
		}

	}
}
void ProducerCli::_ProducerStopMsg(){
	fdfs::OBJECT_ID_4FILE stopmsg;
	stopmsg.IS_PROGRAME_THREAD_STOP=true;
	for(int i=0;i<10;i++){
		G_ItemProduce_Mq.push(stopmsg);
	}
}

void ProducerCli::_ProducerMsg(const fdfs::OBJECT_ID_4SUBDIR &storageInfo) {
	//ready mutex  -->is_finished_hash
	std::vector<fdfs::OBJECT_ID_4FILE> _ret;
	//get sub id
	fdfs::StoragMap * objptr = new fdfs::StoragMap();
	//init log handle
	bool ok = objptr->getfileListOfStoragepath(storageInfo, _ret);
	//insert q
	delete objptr;


	string volumns_str=storageInfo.volumns_id;
	volumns_str.erase(
			remove_if(volumns_str.begin(), volumns_str.end(),
					not1(ptr_fun(static_cast<int (*)(int)>(isdigit)))), volumns_str.end()
	);
	const int volumns_id_int=std::atoi(volumns_str.c_str());

	for (std::vector<fdfs::OBJECT_ID_4FILE>::iterator it = _ret.begin();
			it != _ret.end(); it++) {

		string fileid=it->global_object_id;
		if(volumns_id_int<fdfs::StoragMap::MAX_VOLUMNS_COUNT){
			bpt::value_t* v_pt=new int;
			bpt::key_t* k_ptr=new bpt::key_t(fileid.c_str());
			bool findit=false;
			pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
			findit=BptDelegate::SearchBptByKey(*(G_BptList+volumns_id_int),
					k_ptr,v_pt);
			pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
			delete k_ptr;delete v_pt;
			if(!findit){
				G_ItemProduce_Mq.push(*it);
			}
		}else{
			fdfs::Logger::error(fileid+",\tfail index G_BptList,for index="+volumns_str);
		}
	}



}
/****
 * producer mq end
 */


void ProducerCli::ForkItemConsumerMqHandle(){
	//single
	//	pthread_t consume_t;
	//	fdfs::Thread_Info msg_produce;
	//	int tid=::pthread_create(&consume_t,NULL,&ProducerCli::ListenItemConsumerMq,0);
	//	std::cout<<"create consume"<<tid<<std::endl;
	//	::pthread_join(consume_t,NULL);

	int max_c=MAX_CONSUME_THREAD_NUM;

	fdfs::Logger::debug("consume thread num"+std::to_string(max_c));
	pthread_t pool[max_c];
	//pthread_t log_thread;
	fdfs::Thread_Info msg[max_c];
	for (int i = 0; i < max_c; i++) {
		msg[i].thread_id = i;
		msg[i].thread_msg = "w";
		::pthread_create(&pool[i], NULL, &ProducerCli::ListenItemConsumerMq, &msg[i]);
	}
	for (int i = 0; i < max_c; i++) {
		pthread_join(pool[i], NULL);
	}

}
void* ProducerCli::ListenItemConsumerMq(void*){
	while(true){
		if(!G_ItemProduce_Mq.empty()){
			fdfs::OBJECT_ID_4FILE msg=G_ItemProduce_Mq.try_pop();
			//exit char????
			if(msg.IS_PROGRAME_THREAD_STOP==true){
				G_ItemProduce_Mq.push(msg);//push back
				fdfs::Logger::debug("exit,for consume finished \n");
				pthread_exit(NULL);// producer exit
				break;
			}else{
				//normal consume
				_ConsumeItem(msg);
			}
		}else{
			pthread_cond_signal(&G_Condition_variable);//wake up producer to working
		}
	}


//	return NULL;
}
void ProducerCli::_ConsumeItem(const fdfs::OBJECT_ID_4FILE &info){

	if(info.dir_info_ptr.physical_store_path.empty()){
		return ;
	}
	const std::string physical_fullpath =
					info.dir_info_ptr.physical_store_path + "/"
							+ info.dir_info_ptr.subdir_volumns + "/"
							+ info.filename;
	sendlist.insert(std::make_pair(info.global_object_id,physical_fullpath));


	std::map<std::string,std::string> sendlist;
	senderPtr->uploadItem(sendlist);

	//lsm log
	char buffer[fdfs::MAX_BUFFER_SIZE];
	int lens=sprintf(buffer,"%s\t%s\t%d\n",
			msg.dir_info_ptr.volumns_id.c_str(),
			msg.global_object_id.c_str(),
			1);
	string retmsg(buffer,lens);

	G_ResultLogMq.push(retmsg); //callback
}



/****
 * result log mq start
 */
void ProducerCli::ForkResultMqHandle() {
	pthread_t consume_t;
	fdfs::Thread_Info msg_produce;
	::pthread_create(&consume_t,NULL,&ProducerCli::ListenResultMqByThread,0);
	//::pthread_join(consume_t,NULL);
	pthread_detach(consume_t);
	return ;
}

void* ProducerCli::ListenResultMqByThread(void*) {
	while(true){
		string msgstr;
		G_ResultLogMq.wait_and_pop(msgstr);
		if(!msgstr.empty()){
			//index
			vector<string> msgInfo= fdfs::split(msgstr,'\t');
			//parse msg
			string jobid=msgInfo[0];
			string fileid=msgInfo[1];
			string status=msgInfo[2];

			jobid.erase(
					remove_if(jobid.begin(), jobid.end(),
							not1(ptr_fun(static_cast<int (*)(int)>(isdigit)))), jobid.end()
			);
			status.erase(
					remove_if(status.begin(), status.end(),
							not1(ptr_fun(static_cast<int (*)(int)>(isdigit)))), status.end()
			);

			int index=std::stoi(jobid);
			int status_int=std::stoi(status);
			// lock
			pthread_mutex_lock(&G_bplus_tree_Ptrmutex);
			BptDelegate::InsertAndUpdateBptfromData(*(G_BptList+index),fileid,status_int);
			pthread_mutex_unlock(&G_bplus_tree_Ptrmutex);
		}
	}
}

/****
 * result mq end
 */

}
