// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload::HistoryScraper
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include "Main.hpp"
#include "Worker.hpp"

namespace tgvisd::Main::Preload::HistoryScraper {

Main::Main(void)
{
}


mbp_ret_t Main::initPreload(tgvisd::Main::Main *main)
{
	main->incRef();
	std::thread worker = std::thread([this, main](void){
		this->run(main);
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


void Main::run(tgvisd::Main::Main *main)
{
	Worker *wrk = new Worker(main);
	while (likely(!main->stopUpdate()))
		wrk->run();
}

} /* namespace tgvisd::Modules::Preload */
