// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#if defined(__linux__)
#  include <pthread.h>
#endif

#include <cstdio>
#include <limits>
#include <iostream>
#include "HistoryScraper.hpp"

namespace tgvisd::Modules::Preload {


HistoryScraper::HistoryScraper(void)
{
}


mbp_ret_t HistoryScraper::initPreload(tgvisd::Main::Main *main)
{
	main->incRef();
	std::thread worker = std::thread([this, main](void){
		this->worker(main);
		main->decRef();
	});

#if defined(__linux__)
	{
		char trname[32];
		pthread_t pt = worker.native_handle();
		snprintf(trname, sizeof(trname), "history-scraper");
		pthread_setname_np(pt, trname);
	}
#endif
	worker.detach();
	return MBP_CONTINUE;
}


void HistoryScraper::worker(tgvisd::Main::Main *main)
{
	while (likely(!main->stopUpdate()))
		gatherMsg(main);
}


void HistoryScraper::gatherMsg(tgvisd::Main::Main *main)
{
	
}


} /* namespace tgvisd::Modules::Preload */
