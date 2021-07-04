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

#include <mutex>
#include <queue>
#include <thread>
#include <cassert>
#include <condition_variable>

#define __must_hold(MUTEX)
#define __releases(MUTEX)
#define __acquires(MUTEX)

namespace tgvisd::Main {

class Main;

class Worker
{
public:
	constexpr static uint32_t maxQueue = 15;

	Worker(void);
	~Worker(void);
	void __construct(Main *main, uint32_t idx, bool isPrimaryThread);
	void spawn(void);
	void close(void);

	inline void sendUpdateQueue(td_api::updateNewMessage &update)
	{
		assert(updateCond_);
		assert(updateMutex_);

		updateMutex_->lock();
		updateQueue_.push(std::move(update));
		updateMutex_->unlock();
		updateCond_->notify_one();
	}

	inline uint32_t getIndex(void)
	{
		return idx_;
	}

	inline bool isOnline(void)
	{
		return isOnline_;
	}

	inline bool hasUpdate(void)
	{
		return hasUpdate_;
	}

	inline bool stopEventLoop(void)
	{
		return stopEventLoop_;
	}

	inline bool isPrimaryThread(void)
	{
		return isPrimaryThread_;
	}

	inline void setAcceptingQueue(bool val)
	{
		isAcceptingQueue_ = val;
	}

	inline bool isAcceptingQueue(void)
	{
		return isAcceptingQueue_;
	}

	inline std::thread *getThread(void)
	{
		return thread_;
	}

	inline std::mutex *getupdateMutex(void)
	{
		return updateMutex_;
	}

	inline std::condition_variable *getUpdateCond(void)
	{
		return updateCond_;
	}

private:
	Main *main_;
	uint32_t idx_;

	bool isOnline_ = false;
	bool hasUpdate_ = false;
	bool stopEventLoop_ = true;
	bool isPrimaryThread_ = false;
	bool isAcceptingQueue_ = false;

	std::thread *thread_ = nullptr;
	std::mutex *updateMutex_ = nullptr;
	std::condition_variable *updateCond_ = nullptr;
	td_api::updateNewMessage update_;

	void doSpawn(void);
	void runWorker(void);
	void processQueue(td_api::updateNewMessage update);
	void handleQueue(std::unique_lock<std::mutex> &lock);
	bool waitForEvent(std::unique_lock<std::mutex> &lock);
	void internalWorker(void);
	void internalWorkerPrimary(void);
};


} /* namespace tgvisd::Main */


#endif /* #ifndef TGVISD__MAIN__WORKER_HPP */
