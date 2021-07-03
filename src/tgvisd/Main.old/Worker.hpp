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
#include <mutex>
#include <condition_variable>

namespace tgvisd::Main {

class Main;

class Worker
{
public:
	Worker(void);
	~Worker(void);
	Worker(Main *main, uint32_t idx, bool isPrimaryThread);
	void spawn(void);

	inline bool isBusy(void)
	{
		return isBusy_;
	}

	inline bool isOnline(void)
	{
		return isOnline_;
	}

	inline uint32_t getIndex(void)
	{
		return idx_;
	}

	inline void sendWorkQueue(td_api::updateNewMessage *update)
	{
		isBusy_ = true;
		hasUpdate_ = true;
		update_ = update;
	}

	inline std::mutex *getMutex(void)
	{
		return updateMutex_;
	}

	inline std::condition_variable *getCond(void)
	{
		return upCond_;
	}

private:
	Main *main_;
	uint32_t idx_;
	std::thread *thread_ = nullptr;

	bool stop_ = false;
	bool isBusy_ = false;
	bool isOnline_ = false;
	bool hasUpdate_ = false;
	bool isPrimaryThread_ = false;
	std::mutex *updateMutex_ = nullptr;
	std::condition_variable *upCond_ = nullptr;
	td_api::updateNewMessage *update_ = nullptr;

	void runThread(void);
	void handleEvent(void);
	bool waitForEvent(std::unique_lock<std::mutex> &lock);
	void internalWorker(std::unique_lock<std::mutex> &lock);
	void internalWorkerPrimary(std::unique_lock<std::mutex> &lock);
};


} /* namespace tgvisd::Main */


#endif /* #ifndef TGVISD__MAIN__WORKER_HPP */
