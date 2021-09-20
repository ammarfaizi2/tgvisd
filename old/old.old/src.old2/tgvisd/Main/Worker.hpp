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


namespace tgvisd::Main {


class Main;


class Worker
{
public:
	Worker(void);
	~Worker(void);
	void __construct(Main *main, uint32_t idx, bool isPrimaryWorker);
	void spawn(void);
	void close(void);

	inline void sendUpdate(td_api::updateNewMessage &update)
	{
		assert(updateCond_);
		assert(updateMutex_);
		assert(hasUpdate_ == false);

		updateMutex_->lock();
		update_ = std::move(update);
		hasUpdate_ = true;
		updateMutex_->unlock();
		updateCond_->notify_one();
	}

	inline uint32_t getIndex(void)
	{
		return idx_;
	}

	inline bool isPrimaryWorker(void)
	{
		return isPrimaryWorker_;
	}

	inline std::mutex *getUpdateMutex(void)
	{
		return updateMutex_;
	}

	inline std::condition_variable *getUpdateCond(void)
	{
		return updateCond_;
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

private:
	Main *main_ = nullptr;
	uint32_t idx_ = ~0;
	bool isPrimaryWorker_ = false;

	td_api::updateNewMessage update_;

	std::thread *thread_ = nullptr;
	std::mutex *updateMutex_ = nullptr;
	std::condition_variable *updateCond_ = nullptr;

	bool isOnline_ = false;
	bool hasUpdate_ = false;
	bool stopEventLoop_ = true;

	void doSpawn(void);
	void runWorker(void);
	bool waitForEvent(std::unique_lock<std::mutex> &lock);
	void handleUpdate(std::unique_lock<std::mutex> &lock);
	void internalWorker(std::unique_lock<std::mutex> &lock);
	void internalWorkerPrimary(std::unique_lock<std::mutex> &lock);
};


} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__WORKER_HPP */
