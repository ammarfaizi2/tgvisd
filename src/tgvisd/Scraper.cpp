// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#if defined(__linux__)
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
	pr_notice("In scraper...");
}


} /* namespace tgvisd */
