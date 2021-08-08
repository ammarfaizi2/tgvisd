// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <tgvisd/Main.hpp>
#include <tgvisd/Scraper.hpp>

#if defined(__linux__)
#include <signal.h>
#include <unistd.h>


namespace tgvisd {

/* 
 * Interrupt handler for Linux only.
 */
volatile bool stopEventLoop = false;
static std::mutex sig_mutex;
static volatile bool is_sighandler_set = false;


static void main_sighandler(int sig)
{
	if (!stopEventLoop) {
		printf("\nGot an interrupt signal %d\n", sig);
		stopEventLoop = true;
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


static void updateNewMessage(Main *main, td_api::updateNewMessage &update);


Main::Main(uint32_t api_id, const char *api_hash, const char *data_path):
	td_(api_id, api_hash, data_path)
{
#if defined(__linux__)
	set_interrupt_handler();
#endif

	td_.callback.updateNewMessage = [this](td_api::updateNewMessage &update){
		updateNewMessage(this, update);
	};

	pr_notice("Spawning logger thread...");
	loggerThread_ = new std::thread([this]{
		
	});

	pr_notice("Spawning scraper thread...");
	scraperThread_ = new std::thread([this]{
		Scraper *st = new Scraper(this, this->loggerThread_);
		st->run();
		delete st;
	});
}


Main::~Main(void)
{
	if (loggerThread_) {
		pr_notice("Waiting for logger thread to exit...");
		loggerThread_->join();
		delete loggerThread_;
	}

	if (scraperThread_) {
		pr_notice("Waiting for scraper thread to exit...");
		scraperThread_->join();
		delete scraperThread_;
	}

	pr_notice("Syncing...");
	td_.close();

#if defined(__linux__)
	sync();
#endif
}


int Main::run(void)
{
	constexpr int timeout = 1;

	td_.loop(timeout);
	isReady_ = true;

	while (true) {

#if defined(__linux__)
		/*
		 * We have a signal handler on Linux.
		 *
		 * This can ensure the destructor is called
		 * when we get interrupted. For example, if
		 * the user presses CTRL + C (got SIGINT).
		 */
		if (unlikely(stopEventLoop))
			break;
#endif
		td_.loop(timeout);
	}

	return 0;
}


static void updateNewMessage(Main *main, td_api::updateNewMessage &update)
{

}


} /* namespace tgvisd */
