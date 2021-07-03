// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__WORKER_HPP
#define TGVISD__MAIN__WORKER_HPP

#include <thread>

namespace tgvisd::Main {

class Main;

class Worker
{
public:
	Worker(void);
	~Worker(void);
	Worker(Main *main, uint32_t idx);
	void spawn(void);

	inline bool isBusy(void) {
		return isBusy_;
	}

	inline bool isOnline(void) {
		return isOnline_;
	}

	inline uint32_t getIndex(void) {
		return idx_;
	}

private:
	Main *main_;
	uint32_t idx_;
	std::thread *thread_ = nullptr;

	bool stop_ = false;
	bool isBusy_ = false;
	bool isOnline_ = false;

	void internalWorker(void);
};


} /* namespace tgvisd::Main */


#endif /* #ifndef TGVISD__MAIN__WORKER_HPP */
