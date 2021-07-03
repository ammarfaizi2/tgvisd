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
#include "Worker.hpp"

namespace tgvisd::Main {

using namespace std::chrono_literals;

Worker::Worker(void):
	main_(nullptr)
{
}


Worker::~Worker(void)
{
	if (!main_)
		return;

	if (!stopEventLoop_) {
		/*
		 * Hmmm... event loop is still running.
		 * Let's close it!
		 */
		printf("Waiting for event loop to exit (thread=%u)...\n", idx_);
		this->close();
		assert(stopEventLoop_);
	}

	if (thread_) {
		delete thread_;
	}

	if (updateMutex_)
		delete updateMutex_;

	if (updateCond_)
		delete updateCond_;

	main_ = nullptr;
	thread_ = nullptr;
	updateCond_ = nullptr;
	updateMutex_ = nullptr;
}


void Worker::__construct(Main *main, uint32_t idx, bool isPrimaryThread)
{
	assert(main);

	idx_ = idx;
	main_ = main;
	isPrimaryThread_ = isPrimaryThread;
	updateCond_ = new std::condition_variable;
	updateMutex_ = new std::mutex;
}


void Worker::close(void)
{
	/*
	 * Must null check, we may not have a thread.
	 */
	stopEventLoop_ = true;
	if (thread_) {
		updateMutex_->lock();
		updateCond_->notify_one();
		updateMutex_->unlock();
		thread_->join();
		needJoin_ = false;
	}
}


void Worker::spawn(void)
{
	/*
	 * We must have Main::Main instance!
	 */
	assert(main_);

	if (isOnline_) {
		/*
		 * We are having an online thread.
		 * No need to spawn it!
		 */
		assert(thread_);
		return;
	}


	if (thread_) {
		/*
		 * We have already had std::thread instance.
		 * But it goes offline, let's recreate it!
		 */
		assert(needJoin_ == true);
		assert(stopEventLoop_ == true);
		thread_->join();
		needJoin_ = false;
		delete thread_;
		thread_ = nullptr;
	}

	updateMutex_->lock();
	isOnline_ = true;
	stopEventLoop_ = false;
	printf("Spawning thread %u...\n", idx_);
	thread_ = new std::thread([this](void) {
		this->runThread();
		this->isOnline_ = false;
		assert(stopEventLoop_ == true);
	});
	updateMutex_->unlock();
}


void Worker::runThread(void)
{
	std::unique_lock<std::mutex> lock(*updateMutex_);

	printf("Thread %u is ready\n", idx_);

	if (isPrimaryThread_)
		internalWorkerPrimary(lock);
	else
		internalWorker(lock);

	printf("Thread %u is exiting...\n", idx_);
	updateCond_->notify_one();
	needJoin_ = true;
}


bool Worker::waitForEvent(std::unique_lock<std::mutex> &lock)
{
	return updateCond_->wait_for(lock, 500ms, [this](void){
		return this->hasUpdate_ || this->stopEventLoop_;
	});
}


void Worker::handleEvent()
{
	printf("Thread %u has an update\n", idx_);
	std::cout << to_string(update_) << std::endl;
	sleep(1);
	printf("Thread %u has finished its job\n", idx_);


	/*
	 * Ok, I am free now, can accept more update :)
	 */
	hasUpdate_ = false;
	main_->freeThreadIndex(idx_);
}


void Worker::internalWorkerPrimary(std::unique_lock<std::mutex> &lock)
{
	while (__builtin_expect(!stopEventLoop_, 1)) {
		/*
		 * I am a primary thread, I don't
		 * care about timeout. I am always
		 * online until the program ends.
		 */
		waitForEvent(lock);

		if (__builtin_expect(stopEventLoop_, 0))
			break;

		if (!hasUpdate_)
			continue;

		handleEvent();
	}
}


void Worker::internalWorker(std::unique_lock<std::mutex> &lock)
{
	uint32_t timeoutCounter = 0;
	constexpr uint32_t maxTimeoutCount = 20;

	while (__builtin_expect(!stopEventLoop_, 1)) {

		if (!waitForEvent(lock)) {
			if (++timeoutCounter < maxTimeoutCount)
				continue;

			/*
			 * I have reached my absolute timeout
			 * while waiting for event. I am not
			 * a primary thread neither.
			 *
			 * So it is fine for me to exit because
			 * there is no activity for me.
			 *
			 * Don't worry, I will be resurrected
			 * when we have a heavy load :)
			 *
			 * Sayonara!
			 */
			stopEventLoop_ = true;
			printf("Closing thread %u due to inactivity...\n", idx_);
			break;
		}

		if (__builtin_expect(stopEventLoop_, 0))
			break;

		if (!hasUpdate_)
			continue;

		handleEvent();
		timeoutCounter = 0;
	}
}


} /* namespace tgvisd::Main */
