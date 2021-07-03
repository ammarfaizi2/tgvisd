// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <thread>
#include <iostream>
#include <assert.h>

#if defined(__linux__)
#include <signal.h>
#include <unistd.h>
#endif

#include "Main.hpp"
#include "Worker.hpp"

namespace tgvisd::Main {

#if defined(__linux__)
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


Main::Main(uint32_t api_id, const char *api_hash, const char *data_path):
	td_(api_id, api_hash, data_path)
{
	unsigned int hc;

#if defined(__linux__)
	set_interrupt_handler();
#endif

	td_.callback.updateNewMessage = [this](td_api::updateNewMessage &update){
		this->updateNewMessage(update);
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
	threads_ = new Worker[max_thread_num];
	for (size_t i = 0; i < max_thread_num; i++) {
		threads_[i] = Worker(this, i);
		if (i < hc)
			threads_[i].spawn();
	}

	for (size_t i = max_thread_num; i--;)
		assert(freeThread(i) == (int64_t)i);
}


Main::~Main(void)
{
	td_.close();
	if (threads_)
		delete[] threads_;

#if defined(__linux__)
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
		 * the user presses CTRL + C (got SIGINT)
		 */
		if (__builtin_expect(is_signaled, 0))
			break;
#endif	

		td_.loop(timeout);
	}
}


void Main::updateNewMessage(td_api::updateNewMessage &update)
{
	td_api::object_ptr<td::td_api::message> &msg = update.message_;

	if (msg->content_->get_id() != td_api::messageText::ID) {
		/* Skip non text message. */
		return;
	}

	auto &message = update.message_;
	auto &content = *message->content_;


	std::cout << to_string(update) << std::endl;

	// switch (message->content_->get_id()) {
	// case td_api::messageText::ID: {
	// 	auto &text = static_cast<td_api::messageText &>(content).text_;
	// 	std::cout << text->text_ << std::endl;
	// 	break;
	// }
	// }
}


} /* namespace tgvisd::Main */
