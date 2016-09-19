/*
 * mq.h
 *
 *  Created on: Aug 10, 2016
 *      Author: a11
 */

#ifndef SRC_FDFS2QQ_MQ_H_
#define SRC_FDFS2QQ_MQ_H_
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace fdfs2qq {

template<typename T>
class ConcurrentQueueUnique_lock {
public:

	T wait_pop() {
		std::unique_lock < std::mutex > mlock(mutex_);
		while (queue_.empty()) {
			cond_.wait(mlock);
		}
		auto val = queue_.front();
		queue_.pop();
		return val;
	}
	bool empty() {
		std::unique_lock < std::mutex > mlock(mutex_);
		bool isempty = queue_.empty();
		return isempty;
	}
	void pop(T& item) {
		std::unique_lock < std::mutex > mlock(mutex_);
		if (!queue_.empty()) {
			item = queue_.front();
			queue_.pop();
		}
	}

	void push(const T& item) {
		std::unique_lock < std::mutex > mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}
	ConcurrentQueueUnique_lock() = default;
	ConcurrentQueueUnique_lock(const ConcurrentQueueUnique_lock&) = delete; // disable copying
	ConcurrentQueueUnique_lock& operator=(const ConcurrentQueueUnique_lock&) = delete; // disable assignment

protected:
	std::queue<T> queue_;
	std::mutex mutex_;
private:
	std::condition_variable cond_;
};

template<typename T>
class concurrent_queue: public ConcurrentQueueUnique_lock<T> {
public:
	bool size() {
		std::unique_lock < std::mutex > mlock(this->mutex_);
		int size =this-> queue_.size();
		return size;
	}
	void try_pop(T& item) {
		std::unique_lock < std::mutex > mlock(this->mutex_);
		if (!this->queue_.empty()) {
			item = this->queue_.front();
			this->queue_.pop();
		}
	}

};

//template<typename Data>
//class concurrent_queue {
//private:
//	std::queue<Data> the_queue;
//	mutable ::pthread_mutex_t the_mutex;
//	::pthread_cond_t the_condition_variable;
//public:
//	void push(Data const& data) {
//
//		pthread_mutex_lock(&the_mutex);
//		the_queue.push(data);
//		pthread_mutex_unlock(&the_mutex);
//		pthread_cond_signal(&the_condition_variable);
//	}
//
//	bool empty() const {
//		pthread_mutex_lock(&the_mutex);
//		bool isempty = the_queue.empty();
//		pthread_mutex_unlock(&the_mutex);
//		return isempty;
//	}
//	int size() const {
//		pthread_mutex_lock(&the_mutex);
//		int size_v = the_queue.size();
//		pthread_mutex_unlock(&the_mutex);
//		return size_v;
//	}
//	Data try_pop() {
//		pthread_mutex_lock(&the_mutex);
//		Data popped_value;
//		if (the_queue.empty()) {
//			pthread_mutex_unlock(&the_mutex);
//			return popped_value;
//		}
//		popped_value = the_queue.front();
//		the_queue.pop();
//		pthread_mutex_unlock(&the_mutex);
//		return popped_value;
//	}
//	Data try_pop(bool &flg) {
//		pthread_mutex_lock(&the_mutex);
//		Data popped_value;
//		if (the_queue.empty()) {
//			flg = false;
//			pthread_mutex_unlock(&the_mutex);
//			return popped_value;
//		}
//
//		popped_value = the_queue.front();
//		the_queue.pop();
//		flg = true;
//		pthread_mutex_unlock(&the_mutex);
//		return popped_value;
//	}
//
//	bool try_pop(Data& popped_value) {
//		pthread_mutex_lock(&the_mutex);
//		if (the_queue.empty()) {
//			pthread_mutex_unlock(&the_mutex);
//			return false;
//		}
//
//		popped_value = the_queue.front();
//		the_queue.pop();
//		pthread_mutex_unlock(&the_mutex);
//		return true;
//	}
//
//	void wait_and_pop(Data& popped_value) {
//		pthread_mutex_lock(&the_mutex);
//		while (the_queue.empty()) {
//			pthread_cond_wait(&the_condition_variable, &the_mutex);
//		}
//
//		popped_value = the_queue.front();
//		the_queue.pop();
//		pthread_mutex_unlock(&the_mutex);
//	}
//
//};

}

#endif /* SRC_FDFS2QQ_MQ_H_ */
