/*
 * mq.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */
#pragma once
#ifndef SRC_FDFS2QQ_MQ_H_
#define SRC_FDFS2QQ_MQ_H_
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace fdfs2qq {

template<typename Data>
class concurrent_queue {
private:
	std::queue<Data> the_queue;
	mutable ::pthread_mutex_t the_mutex;
	::pthread_cond_t the_condition_variable;
public:
	int size() const {
		pthread_mutex_lock(&the_mutex);
		int size_v = the_queue.size();
		pthread_mutex_unlock(&the_mutex);
		return size_v;
	}
	void push(Data const& data) {

		pthread_mutex_lock(&the_mutex);
		the_queue.push(data);
		pthread_mutex_unlock(&the_mutex);
		pthread_cond_signal(&the_condition_variable);
	}

	bool empty() const {
		pthread_mutex_lock(&the_mutex);
		bool isempty = the_queue.empty();
		pthread_mutex_unlock(&the_mutex);
		return isempty;
	}
	Data try_pop(bool &flg) {
		pthread_mutex_lock(&the_mutex);
		Data popped_value;
		if (the_queue.empty()) {
			flg = false;
			pthread_mutex_unlock(&the_mutex);
			return popped_value;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		flg = true;
		pthread_mutex_unlock(&the_mutex);
		return popped_value;
	}

	bool try_pop(Data& popped_value) {
		pthread_mutex_lock(&the_mutex);
		if (the_queue.empty()) {
			pthread_mutex_unlock(&the_mutex);
			return false;
		}
		try{
				popped_value = the_queue.front();
				the_queue.pop();
		}catch(std::exception &ex){

		}
		pthread_mutex_unlock(&the_mutex);
		return true;
	}

	void wait_and_pop(Data& popped_value) {
		pthread_mutex_lock(&the_mutex);
		while (the_queue.empty()) {
			pthread_cond_wait(&the_condition_variable, &the_mutex);
		}

		popped_value = the_queue.front();
		the_queue.pop();
		pthread_mutex_unlock(&the_mutex);
	}

};

}

#endif /* SRC_FDFS2QQ_MQ_H_ */
