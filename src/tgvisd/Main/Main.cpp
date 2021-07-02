// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstdio>
#include <iostream>
#include <signal.h>

#include "Main.hpp"

namespace tgvisd::Main {

#if defined(__linux__)
static std::mutex sig_mutex;
static bool is_signaled = false;
static bool is_sighandler_set = false;
#endif


static void main_sighandler(int sig)
{
	is_signaled = true;
	printf("Got interrupt signal %d\n", sig);
}


Main::Main(uint32_t api_id, const char *api_hash, const char *data_path):
	td_(std::make_shared<tgvisd::Td::Td>(api_id, api_hash, data_path))
{


#if defined(__linux__)
	/*
	 * Use signal interrupt handler to kill the
	 * process gracefully on Linux.
	 *
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
#endif


	td_->callback.updateNewMessage = [this](td_api::updateNewMessage &update) {
		std::cout << "test" << std::endl;
	};
}


Main::~Main(void)
{
	td_->close();
}


void Main::run(void)
{
	constexpr int timeout = 1;

	while (true) {


#if defined(__linux__)
		/*
		 * We have signal handler on Linux.
		 *
		 * This can ensure the destructor is called
		 * when we get interrupted. For example, if
		 * the user presses CTRL + C (got SIGINT)
		 */
		if (__builtin_expect(is_signaled, 0))
			break;
#endif	

		td_->loop(timeout);
	}
}


} /* namespace tgvisd::Main */



int main(int argc, char *argv[])
{
	int ret = 0;



	return ret;
}
