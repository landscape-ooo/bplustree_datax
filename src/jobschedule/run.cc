/*
 * run.cc
 *
 *  Created on: Aug 26, 2016
 *      Author: a11
 */
#include <iostream>
#include "ProducerCli.h"
//#include "ConsumerCli.h"
#include "TrackerCli.h"
using namespace jobschedule;
using namespace std;
int main(int argc, const char *argv[]) {
	char buffer[50];
	int len=std::sprintf(buffer,"%s",argv[1]);
	const std::string  casestr(buffer,len);
	if (casestr == "produce") {
		ProducerCli::Init();
		//fork lsm
//		ProducerCli::LSMBinlog();
		//fork producer
		ProducerCli::ReadyProducer();
		ProducerCli::ForkProducer();

//		ProducerCli::Init();
//		//fork producer
		//ProducerCli::ForkConsumer();
	}else if (casestr == "tracker") {
		TrackerCli::InitBpt();
		TrackerCli::StartRecvBinlog();
		TrackerCli::RegisteEvent();
		//fork producer
	}else{


		std::cout<<"Usage :<produce|tracker>\n";
	}

	return 1;
}

