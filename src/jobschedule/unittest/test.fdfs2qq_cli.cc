/*
 * run.fdfs2qq_cli.cc
 *
 *  Created on: Aug 8, 2016
 *      Author: a11
 */
#include "../FdfsJobschedule.h"
int main (){
	//load config

	jobschedule::ProducerCli::Init();

	//fork producer
	jobschedule::ProducerCli::ForkProducer();


	//fork lsm
	jobschedule::ProducerCli::ForkResultMqHandle();


	//fork consum
	//join!!!!
	jobschedule::ProducerCli::ForkItemConsumerMqHandle();



	fdfs::Logger::debug("exit....");
}


