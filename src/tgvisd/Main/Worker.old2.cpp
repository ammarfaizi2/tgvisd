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


void Worker::__construct(Main *main, uint32_t idx, bool isPrimaryThread)
{
	assert(main);
	assert(updateQueue_.empty());

	idx_ = idx;
	main_ = main;
	isPrimaryThread_ = isPrimaryThread;
	updateCond_ = new std::condition_variable;
	updateMutex_ = new std::mutex;
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


void Worker::doSpawn()
{
	std::unique_lock<std::mutex> lock(*updateMutex_);

	assert(this->isOnline_ == false);

	/*
	 * Spawn a thread for this worker.
	 */
	printf("Spawning thread %u...\n", idx_);
	thread_ = new std::thread([this](void){
		this->stopEventLoop_ = false;
		this->isOnline_ = true;
		this->updateCond_->notify_one();
		this->runWorker();
		this->isOnline_ = false;

		/*
		 * When the worker returns, the event
		 * loop must have stopped.
		 */
		assert(this->stopEventLoop_ == true);
	});

	while (!this->isOnline_) {
		updateCond_->wait(lock, [this](void){
			return this->isOnline_;
		});
	}
}


void Worker::spawn(void)
{
	/*
	 * We must have Main::Main instance at this point!
	 */
	assert(main_);

	if (isOnline_) {
		/*
		 * We are having an online thread.
		 * No need to spawn!
		 */
		assert(thread_);
		return;
	}

	if (thread_) {
		/*
		 * We have already had std::thread instance.
		 * But it goes offline, let's recreate it!
		 */
		assert(stopEventLoop_ == true);
		thread_->join();
		delete thread_;
		thread_ = nullptr;
	}

	doSpawn();
}


void Worker::runWorker(void)
{
	printf("Thread %u is ready!\n", idx_);

	if (isPrimaryThread_)
		internalWorkerPrimary();
	else
		internalWorker();

	assert(stopEventLoop_ == true);
	printf("Thread %u exited!\n", idx_);
}


void Worker::processQueue(td_api::updateNewMessage update)
{
	FILE *handle = fopen("/dev/urandom", "rb");
	unsigned int trand;
	fread(&trand, sizeof(unsigned int), 1, handle);

	trand %= 3;
	printf("Proessing event on thread %u (sleep %u)...\n", idx_, trand);
	sleep(trand);
	printf("Thread %u has finished its job!\n", idx_);
}


void Worker::handleQueue(std::unique_lock<std::mutex> &lock)
	__must_hold(&lock)
{
	while (!updateQueue_.empty()) {
		td_api::updateNewMessage update = std::move(updateQueue_.front());
		updateQueue_.pop();

		/* TODO: Explain why do I unlock the mutex here. */
		lock.unlock();
		processQueue(std::move(update));
		lock.lock();
		printf("Remaining queue on thread %u = %zu\n", idx_, updateQueue_.size());
	}

	if (!isAcceptingQueue_) {
		isAcceptingQueue_ = true;
		main_->addFreeWorker(this);
	}
}


bool Worker::waitForEvent(std::unique_lock<std::mutex> &lock)
	__must_hold(&lock)
{
	return updateCond_->wait_for(lock, 1000ms, [this](void){
		return this->hasUpdate_ || this->stopEventLoop_;
	});
}


void Worker::internalWorkerPrimary(void)
	__acquires(updateMutex_)
	__releases(updateMutex_)
{
	std::unique_lock<std::mutex> lock(*updateMutex_);

	while (__builtin_expect(!stopEventLoop_, 1)) {

		/*
		 * I am a primary thread, I don't
		 * care about timeout. I am always
		 * online until the program ends.
		 */
		if (!waitForEvent(lock))
			continue;

		handleQueue(lock);
	}
}


void Worker::internalWorker(void)
	__acquires(updateMutex_)
	__releases(updateMutex_)
{
	uint32_t timeoutCounter = 0;
	constexpr uint32_t maxTimeoutCount = 20;
	std::unique_lock<std::mutex> lock(*updateMutex_);

	while (__builtin_expect(!stopEventLoop_, 1)) {

		if (!waitForEvent(lock)) {
			if (++timeoutCounter < maxTimeoutCount)
				continue;

			/*
			 * I have reached my absolute timeout
			 * while waiting for events. I am not
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

		handleQueue(lock);
		timeoutCounter = 0;
	}
}


} /* namespace tgvisd::Main */
