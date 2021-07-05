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
#include <atomic>
#include <tgvisd/Td/Td.hpp>
#include <tgvisd/common.hpp>
#include <condition_variable>

namespace tgvisd::Main {

class HistoryScraper;

class Module;

class Worker;

class Main {

public:
	Main(uint32_t api_id, const char *api_hash, const char *data_path);
	~Main(void);
	void run(void);

	Worker *getPrimaryWorker(void);
	void putPrimaryWorker(Worker *worker);

	Worker *getExtraWorker(void);
	void putExtraWorker(Worker *worker);

	inline int32_t incRef(void)
	{
		assert(stopUpdate_ == false);
		return atomic_fetch_add(&myRef_, 1);
	}


	inline int32_t decRef(void)
	{
		int32_t ret = atomic_fetch_sub(&myRef_, 1);
		refCond_.notify_all();
		return ret;
	}

	inline std::mutex *getRefLock(void)
	{
		return &refLock_;
	}

	inline std::condition_variable *getRefCond(void)
	{
		return &refCond_;
	}

	inline tgvisd::Td::Td *getTd(void)
	{
		return &td_;
	}

	inline bool stopUpdate(void)
	{
		return stopUpdate_;
	}

	inline Module *getModule(void)
	{
		return module_;
	}

private:
	tgvisd::Td::Td td_;
	uint32_t maxWorkerNum_;
	uint32_t hardwareConcurrency_;
	Worker *threads_ = nullptr;
	Module *module_ = nullptr;

	/*
	 * These ref(s) are used for reference
	 * counting of preloaded module.
	 *
	 * Before Main::Main is destroyed, we
	 * must wait for all preloaded modules
	 * to be unloaded. These variables are
	 * the communication channel to them.
	 *
	 * Don't destroy Main::Main before
	 * myRef_ is zero.
	 */
	std::atomic<int32_t> myRef_ = 0;
	std::mutex refLock_;
	std::condition_variable refCond_;


	bool stopUpdate_ = false;

	std::mutex primaryWorkerStackMutex;
	std::stack<uint32_t> primaryWorkerStack;

	std::mutex extraWorkerStackMutex;
	std::stack<uint32_t> extraWorkerStack;

	void spawnThreads(void);
};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__MAIN_HPP */
