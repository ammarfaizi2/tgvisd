// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <thread>
#include <cassert>
#include <iostream>

#include "Main.hpp"
#include "Worker.hpp"


#if defined(__linux__)
#  include <signal.h>
#  include <unistd.h>

/* 
 * Interrupt handler for Linux only.
 */
static std::mutex sig_mutex;
static bool is_signaled = false;
static bool is_sighandler_set = false;


static void main_sighandler(int sig)
{
	if (!is_signaled) {
		printf("\nGot an interrupt signal %d\n", sig);
		is_signaled = true;
	}
}


static void set_interrupt_handler(void)
{
	/*
	 * Use signal interrupt handler to kill the
	 * process gracefully on Linux.
	 *
	 * Mutex note:
	 * We may have several instances calling
	 * Main::Main simultaneously, so we need a
	 * mutex here.
	 *
	 * Signal handler is only required to be
	 * set once.
	 */
	sig_mutex.lock();
	if (__builtin_expect(!is_sighandler_set, 0)) {
		signal(SIGINT, main_sighandler);
		signal(SIGHUP, main_sighandler);
		signal(SIGTERM, main_sighandler);
		is_sighandler_set = true;
	}
	sig_mutex.unlock();
}

#endif /* #if defined(__linux__) */


namespace tgvisd::Main {


static void updateNewMessage(Main *main, td_api::updateNewMessage &update);


Main::Main(uint32_t api_id, const char *api_hash, const char *data_path):
	td_(api_id, api_hash, data_path)
{
	unsigned int hc;

#if defined(__linux__)
	set_interrupt_handler();
#endif

	td_.callback.updateNewMessage = [this](td_api::updateNewMessage &update){
		updateNewMessage(this, update);
	};


	/*
	 * Initialize workers.
	 *
	 * Don't spawn all workers at once, only spawn
	 * small number of them. When the online workers
	 * are all busy and we got more queue, then spawn
	 * more workers.
	 */
	hc = std::thread::hardware_concurrency();
	if (!hc)
		hc = 2;

	maxWorkerNum_ = hc * 100;
	hardwareConcurrency_ = hc;
	threads_ = new Worker[maxWorkerNum_];

	pr_debug("Hardware concurrency number is %u", hc);
	pr_debug("Max worker number is %u", maxWorkerNum_);

	for (size_t i = 0; i < maxWorkerNum_; i++) {
		threads_[i].__construct(this, i, i < hc);
		if (i < hc)
			threads_[i].spawn();
	}

	for (size_t i = hc; i--;) {
		assert(threads_[i].getIndex() == i);
		putPrimaryWorker(&threads_[i]);
	}

	for (size_t i = maxWorkerNum_; i-- > hc;) {
		assert(threads_[i].getIndex() == i);
		putExtraWorker(&threads_[i]);
	}

	assert(primaryWorkerStack.size() == hc);
	assert(extraWorkerStack.size() == (maxWorkerNum_ - hc));
}


Main::~Main(void)
{
	stopUpdate_ = true;

	if (threads_)
		delete[] threads_;

	td_.close();

#if defined(__linux__)
	/*
	 * Sync memory to disk to ensure
	 * data consistency.
	 */
	printf("Syncing...\n");
	sync();
#endif
}


void Main::run(void)
{
	constexpr int timeout = 1;
	while (true) {

#if defined(__linux__)
		/*
		 * We have a signal handler on Linux.
		 *
		 * This can ensure the destructor is called
		 * when we get interrupted. For example, if
		 * the user presses CTRL + C (got SIGINT).
		 */
		if (__builtin_expect(is_signaled, 0)) {
			stopUpdate_ = true;
			break;
		}
#endif
		td_.loop(timeout);
	}
}


Worker *Main::getPrimaryWorker(void)
	__acquires(primaryWorkerStackMutex)
	__releases(primaryWorkerStackMutex)
{
	uint32_t idx;
	Worker *ret = nullptr;
	primaryWorkerStackMutex.lock();
	if (likely(!primaryWorkerStack.empty())) {
		idx = primaryWorkerStack.top();
		ret = &threads_[idx];
		primaryWorkerStack.pop();
	}
	primaryWorkerStackMutex.unlock();
	return ret;
}


void Main::putPrimaryWorker(Worker *worker)
	__acquires(primaryWorkerStackMutex)
	__releases(primaryWorkerStackMutex)
{
	uint32_t idx;
	primaryWorkerStackMutex.lock();
	assert(worker->hasUpdate() == false);
	assert(primaryWorkerStack.size() <= hardwareConcurrency_);
	idx = worker->getIndex();
	assert(idx < hardwareConcurrency_);
	primaryWorkerStack.push(idx);
	primaryWorkerStackMutex.unlock();
}


Worker *Main::getExtraWorker(void)
	__acquires(extraWorkerStackMutex)
	__releases(extraWorkerStackMutex)
{
	uint32_t idx;
	Worker *ret = nullptr;
	extraWorkerStackMutex.lock();
	if (likely(!extraWorkerStack.empty())) {
		idx = extraWorkerStack.top();
		ret = &threads_[idx];
		extraWorkerStack.pop();
	}
	extraWorkerStackMutex.unlock();
	return ret;
}


void Main::putExtraWorker(Worker *worker)
	__acquires(extraWorkerStackMutex)
	__releases(extraWorkerStackMutex)
{
	uint32_t idx;
	extraWorkerStackMutex.lock();
	assert(worker->hasUpdate() == false);
	assert(extraWorkerStack.size() <= (maxWorkerNum_ - hardwareConcurrency_));
	idx = worker->getIndex();
	assert(idx >= hardwareConcurrency_);
	extraWorkerStack.push(idx);
	extraWorkerStackMutex.unlock();
}


static void updateNewMessage(Main *main, td_api::updateNewMessage &update)
{
	Worker *worker;

	if (unlikely(main->stopUpdate())) {
		/*
		 * Sorry, we are in the destruction time.
		 *
		 * Don't give it to workers, it might
		 * fault because the workers are being
		 * destroyed at this point!
		 *
		 * What is the bad thing here?
		 * We lose this update.
		 *
		 * TODO: Make sure Td doesn't give any
		 *       update when `stopUpdate_` is
		 *       true, so we don't lose it!
		 */
		pr_debug("Dropping update due to destruction time...");
		return;
	}



	/*
	 * Always prioritize the primary workers
	 * first. If they are all busy, throw it
	 * to the extra workers.
	 */
	worker = main->getPrimaryWorker();
	if (likely(worker))
		goto out_work;


	/*
	 * All primary workers are busy. Let's
	 * give the update to the extra worker.
	 */
	worker = main->getExtraWorker();
	if (likely(worker))
		goto out_work;


	/*
	 * All workers are all busy.
	 *
	 * TODO: Handle extra queue for this event.
	 */
	pr_debug("Dropping update because all threads are busy...");
	return;


out_work:
	worker->sendUpdate(update);
	if (!worker->isOnline())
		/*
		 * The worker is offline, let's give it a life.
		 */
		worker->spawn();
}


} /* namespace tgvisd::Main */
