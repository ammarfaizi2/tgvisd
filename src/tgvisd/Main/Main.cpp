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
	 * small number of it. If the workers are all
	 * busy while queue is being enqueued, then
	 * spawn more workers.
	 */
	hc = std::thread::hardware_concurrency();
	if (!hc)
		hc = 2;

	threads = new Worker[max_thread_num];
	for (size_t i = 0; i < max_thread_num; i++) {
		threads[i].__construct(this, i, i < hc);
		if (i < hc)
			threads[i].spawn();
	}

	for (size_t i = max_thread_num; i--;) {
		threads[i].setAcceptingQueue(true);
		addFreeWorker(&threads[i]);
	}
}


Main::~Main(void)
{
	if (threads)
		delete[] threads;

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


void Main::addFreeWorker(Worker *thread)
	__acquires(&workerStackMutex_)
	__releases(&workerStackMutex_)
{
	workerStackMutex_.lock();
	assert(thread->isAcceptingQueue());
	workerStack_.push(thread->getIndex());
	assert(workerStack_.size() <= max_thread_num);
	workerStackMutex_.unlock();
}


Worker *Main::getFreeWorker(void)
	__acquires(&workerStackMutex_)
	__releases(&workerStackMutex_)
{
	uint32_t idx;
	Worker *ret = nullptr;
	workerStackMutex_.lock();
	if (__builtin_expect(!workerStack_.empty(), 1)) {
		idx = workerStack_.top();
		ret = &threads[idx];

		if (ret->getUpdateQueue()->size() >= Worker::maxQueue) {
			workerStack_.pop();
			ret->setAcceptingQueue(false);
		}
	}
	workerStackMutex_.unlock();
	return ret;
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
		if (__builtin_expect(is_signaled, 0))
			break;
#endif
		td_.loop(timeout);
	}
}


static void updateNewMessage(Main *main, td_api::updateNewMessage &update)
{
	Worker *thread;

	thread = main->getFreeWorker();
	if (__builtin_expect(!thread, 0)) {
		/*
		 * All threads are busy!
		 *
		 * TODO: Handle this event!
		 */
		return;
	}

	thread->sendUpdateQueue(update);

	if (!thread->isOnline())
		thread->spawn();
}


} /* namespace tgvisd::Main */
