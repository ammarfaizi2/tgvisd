// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstdio>
#include <chrono>
#include <iostream>

#include "Main.hpp"
#include "Module.hpp"
#include "Worker.hpp"

#if defined(__linux__)
#  include <pthread.h>
#endif

namespace tgvisd::Main {


using namespace std::chrono_literals;


Worker::Worker(void):
	main_(nullptr)
{
}


void Worker::__construct(Main *main, uint32_t idx, bool isPrimaryWorker)
{
	assert(main);

	main_ = main;
	idx_ = idx;
	isPrimaryWorker_ = isPrimaryWorker;
	updateMutex_ = new std::mutex;
	updateCond_ = new std::condition_variable;
}


Worker::~Worker(void)
{
	if (!main_)
		return;

	if (thread_)
		this->close();

	if (updateMutex_)
		delete updateMutex_;

	if (updateCond_)
		delete updateCond_;

	main_ = nullptr;
	updateCond_ = nullptr;
	updateMutex_ = nullptr;
}


void Worker::close(void)
{
	stopEventLoop_ = true;
	if (thread_) {
		updateCond_->notify_one();
		thread_->join();
		delete thread_;
		thread_ = nullptr;
	}
}


void Worker::spawn(void)
{
	assert(updateMutex_);
	assert(updateCond_);

	if (unlikely(isOnline_))
		/*
		 * We already have online thread.
		 * No need to spawn!
		 */
		return;


	if (thread_) {
		/*
		 * We have the std::thread instance.
		 * But the thread has gone offline.
		 * Let's recreate it!
		 */

		/*
		 * Offline thread must not run
		 * the event loop!
		 */
		assert(stopEventLoop_ == true);

		thread_->join();
		delete thread_;
		thread_ = nullptr;
	}

	doSpawn();
}


void Worker::doSpawn(void)
	__acquires(updateMutex_)
	__releases(updateMutex_)
{
	std::unique_lock<std::mutex> lock(*updateMutex_);

	assert(isOnline_ == false);

	pr_debug("Spawning thread %u...", idx_);
	thread_ = new std::thread([this](void){

		this->stopEventLoop_ = false;
		this->isOnline_ = true;

		/*
		 * Hey! Be careful!
		 *
		 * this->thread_ may be a nullptr at this point.
		 *
		 * When the constructor of std::thread is called,
		 * we are spawned. However, the object may haven't
		 * been assigned to this->thread_.
		 *
		 * Don't race with the parent thread. Let's wait!
		 */
		{
			std::unique_lock<std::mutex> lock(*this->updateMutex_);
			while (this->thread_ == nullptr) {
				this->updateCond_->wait(lock, [this](void){
					return this->thread_ != nullptr;
				});
			}
		}


#if defined(__linux__)

		{
			char trname[32];
			pthread_t pt = this->thread_->native_handle();
			snprintf(trname, sizeof(trname), "wrk-%04u", this->idx_);
			pthread_setname_np(pt, trname);
		}
#endif


		/*
		 * I am ready, notify the calling thread!
		 */
		this->updateCond_->notify_one();


		this->runWorker();
		assert(isOnline_ == false);

		/*
		 * When the worker returns, the event
		 * loop must have stopped.
		 */
		assert(this->stopEventLoop_ == true);
	});


	/*
	 * In any way, this->thread_ cannot be nullptr
	 * at this point!
	 */
	updateCond_->notify_one();


	/*
	 * Wait for the thread to be ready.
	 */
	while (!isOnline_) {
		updateCond_->wait(lock, [this](void){
			return this->isOnline_;
		});
	}
	pr_debug("Thread %u is ready!", idx_);
}


void Worker::runWorker(void)
	__acquires(updateMutex_)
	__releases(updateMutex_)
{
	std::unique_lock<std::mutex> lock(*updateMutex_);
	assert(isOnline_ == true);

	if (isPrimaryWorker_)
		internalWorkerPrimary(lock);
	else
		internalWorker(lock);

	isOnline_ = false;
	assert(hasUpdate_ == false);
	assert(stopEventLoop_ == true);
	pr_debug("Thread %u exited!", idx_);
}


void Worker::handleUpdate(std::unique_lock<std::mutex> &lock)
	__must_hold(updateMutex_)
{
	pr_debug("Proessing event on thread %u...", idx_);
	main_->getModule()->enumerate(main_, update_);
	pr_debug("Thread %u has finished its job!", idx_);
	hasUpdate_ = false;
}


bool Worker::waitForEvent(std::unique_lock<std::mutex> &lock)
	__must_hold(updateMutex_)
{
	return updateCond_->wait_for(lock, 10000ms, [this](void){
		return this->hasUpdate_ || this->stopEventLoop_;
	});
}


void Worker::internalWorkerPrimary(std::unique_lock<std::mutex> &lock)
	__must_hold(updateMutex_)
{
	while (likely(!stopEventLoop_)) {

		/*
		 * I am a primary worker, I must always
		 * be online and wait for events even
		 * though there is no event comming to
		 * me at the moment (and in the near
		 * future).
		 *
		 * I don't really care about timeout.
		 *
		 * I only stop if the `main_` stops me.
		 */
		if (!waitForEvent(lock))
			continue;

		if (unlikely(stopEventLoop_))
			break;

		assert(hasUpdate_ == true);
		handleUpdate(lock);
		assert(hasUpdate_ == false);
		main_->putPrimaryWorker(this);
	}
}


void Worker::internalWorker(std::unique_lock<std::mutex> &lock)
	__must_hold(updateMutex_)
{
	uint32_t timeoutCounter = 0;
	constexpr uint32_t maxTimeoutCount = 20; /* Absolute timeout */

	while (likely(!stopEventLoop_)) {

		if (!waitForEvent(lock)) {
			if (likely(++timeoutCounter < maxTimeoutCount))
				continue;

			/*
			 * I am not a primary worker and I have
			 * reached the maximum timeout while
			 * waiting for events.
			 *
			 * Therefore, it is fine for me to exit
			 * because there is no activity for me
			 * at the moment (and probably in the
			 * near future).
			 *
			 * Don't worry, I will be resurrected
			 * when we have a heavy load :)
			 *
			 * Sayonara!
			 */
			stopEventLoop_ = true;
			pr_debug("Closing worker %u (no activity)...", idx_);
			break;
		}

		if (unlikely(stopEventLoop_))
			break;

		assert(hasUpdate_ == true);
		handleUpdate(lock);
		assert(hasUpdate_ == false);
		main_->putExtraWorker(this);
		timeoutCounter = 0;
	}
}


} /* namespace tgvisd::Main */
