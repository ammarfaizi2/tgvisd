// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__MAIN_HPP
#define TGVISD__MAIN__MAIN_HPP

#include <stack>
#include <tgvisd/Td/Td.hpp>

namespace tgvisd::Main {

class Worker;

class Main {
public:
	Main(uint32_t api_id, const char *api_hash, const char *data_path);
	~Main(void);
	void run(void);
private:
	constexpr static size_t max_thread_num = 128;

	tgvisd::Td::Td td_;
	Worker *threads_ = nullptr;

	/* Mutex to lock the stack below. */
	std::mutex thread_stk_mutex_;

	/* It saves the free thread indexes. */
	std::stack<uint32_t> thread_stk_;


	inline int64_t freeThread(uint32_t idx) {
		int64_t ret;
		thread_stk_mutex_.lock();
		if (thread_stk_.size() < max_thread_num) {
			thread_stk_.push(idx);
			ret = (int64_t)idx;
		} else {
			/*
			 * Stack is full, means they are all free.
			 * How come you call freeThread?
			 */
			ret = -1;
		}
		thread_stk_mutex_.unlock();
		return ret;
	}

	inline int64_t getFreeThreadIdx(void) {
		int64_t ret;
		thread_stk_mutex_.lock();

		if (__builtin_expect(!thread_stk_.empty(), 1)) {
			ret = (int64_t)thread_stk_.top();
			thread_stk_.pop();
		} else {
			ret = -1;
		}

		thread_stk_mutex_.unlock();
		return ret;
	}

	void spawnThreads(void);
	void updateNewMessage(td_api::updateNewMessage &update);
};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__MAIN_HPP */
