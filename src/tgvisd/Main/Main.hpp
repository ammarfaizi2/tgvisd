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

	inline size_t incRef(void)
	{
		return atomic_fetch_add(&myRef_, 1);
	}

	inline size_t decRef(void)
	{
		return atomic_fetch_sub(&myRef_, 1);
	}

private:
	tgvisd::Td::Td td_;
	uint32_t maxWorkerNum_;
	uint32_t hardwareConcurrency_;
	Worker *threads_ = nullptr;
	Module *module_ = nullptr;
	HistoryScraper *hs_ = nullptr;
	std::atomic<size_t> myRef_ = 0;

	bool stopUpdate_ = false;

	std::mutex primaryWorkerStackMutex;
	std::stack<uint32_t> primaryWorkerStack;

	std::mutex extraWorkerStackMutex;
	std::stack<uint32_t> extraWorkerStack;

	void spawnThreads(void);
};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__MAIN_HPP */
