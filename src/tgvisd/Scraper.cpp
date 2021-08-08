// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#if defined(__linux__)
#include <unistd.h>
#include <pthread.h>
#endif

#include <tgvisd/Scraper.hpp>

namespace tgvisd {


Scraper::Scraper(Main *main):
	main_(main)
{
}


Scraper::Scraper(Main *main, std::thread *threadPtr):
	main_(main)
{
#if defined(__linux__)
	pthread_t pt = threadPtr->native_handle();
	pthread_setname_np(pt, "scraper");
#endif
}


Scraper::~Scraper(void)
{
}


void Scraper::run(void)
{
	while (!main_->isReady() && !main_->getStop()) {
		pr_notice("In scraper (waiting for main thread to be ready)...");
		sleep(1);
	}

	while (!main_->getStop()) {
		pr_notice("In scraper...");
		sleep(1);
	}

	pr_notice("Closing scraper...");
}


} /* namespace tgvisd */
