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
	printf("dummy constructor is called\n");
}


Worker::Worker(Main *main, uint32_t idx, bool isPrimary):
	main_(main),
	idx_(idx),
	isPrimaryThread_(isPrimary)
{
	upCond_ = new std::condition_variable;
	updateMutex_ = new std::mutex;
}


Worker::~Worker()
{
	if (!main_)
		/*
		 * I was constructed from dummy
		 * constructor. So there is nothing
		 * to destroy.
		 */
		return;


	stop_ = true;
	updateMutex_->lock();
	upCond_->notify_one();
	updateMutex_->unlock();

	sleep(1);

	updateMutex_->lock();
	delete upCond_;
	delete updateMutex_;

	if (thread_) {
		printf("Waiting for thread %u...\n", idx_);
		thread_->join();
		printf("Thread %u has been joined\n", idx_);
		delete thread_;
	}
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
}


void Worker::spawn(void)
{
	if (isOnline_)
		return;

	if (thread_) {
		/*
		 * We already have thread instance in this
		 * context. Must stop it before recreate a
		 * new one!
		 */
		updateMutex_->lock();
		stop_ = true;
		upCond_->notify_one();
		updateMutex_->unlock();

		thread_->join();
		delete thread_;
	}


	updateMutex_->lock();
	isOnline_ = true;
	printf("Spawning thread %u...\n", idx_);
	thread_ = new std::thread([this](void) {
		this->runThread();
		this->isOnline_ = false;
	});
	updateMutex_->unlock();
}


bool Worker::waitForEvent(std::unique_lock<std::mutex> &lock)
{
	return upCond_->wait_for(lock, 500ms, [this]{
		return this->hasUpdate_ || this->stop_;
	});
}


void Worker::handleEvent()
{
	printf("Thread %u has an update\n", idx_);
	std::cout << to_string(*update_) << std::endl;
	sleep(3);
	printf("Thread %u has finished its job\n", idx_);


	/*
	 * Ok, I am free now, can accept more update :)
	 */
	isBusy_ = false;
	hasUpdate_ = false;
	main_->freeThread(idx_);
}


void Worker::internalWorkerPrimary(std::unique_lock<std::mutex> &lock)
{
	while (__builtin_expect(!stop_, 1)) {
		/*
		 * I am a primary thread, I don't
		 * care about timeout. I am always
		 * online until the program ends.
		 */
		waitForEvent(lock);

		if (__builtin_expect(stop_, 0))
			break;

		if (!hasUpdate_)
			continue;

		handleEvent();
	}
}


void Worker::internalWorker(std::unique_lock<std::mutex> &lock)
{
	uint32_t timeoutCounter = 0;
	constexpr uint32_t maxTimeoutCount = 300;

	while (__builtin_expect(!stop_, 1)) {

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
			stop_ = true;
			break;
		}

		if (__builtin_expect(stop_, 0))
			break;

		if (!hasUpdate_)
			continue;

		handleEvent();
		timeoutCounter = 0;
	}
}


} /* namespace tgvisd::Main */
