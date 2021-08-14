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
	if (chatScraper_) {
		delete chatScraper_;
		chatScraper_ = nullptr;
	}
}


static void runScrapers(Scraper *sc)
{
	// auto st = db->prepare("SELECT eeee;");
	// st->execute();
	// if (auto row = st->fetch()) {
	// 	int ft;
	// 	char buffer[64];
	// 	auto stmt = st->getStmt();
	// 	size_t len = sizeof(buffer);

	// 	ft = mysqlx_get_bytes(row, 0, 0, buffer, &len);
	// 	mysql_fetch_chk(ft, stmt);
	// 	pr_notice("buf = %s (len = %zu)", buffer, len);
	// }
	// sleep(1);

	sc->chatScraper_ = new tgvisd::Scrapers::ChatScraper(sc);

	std::thread chatScraper([sc]{
		sc->chatScraper_->run();
	});
	chatScraper.join();
}


void Scraper::run(void)
{
	while (!main_->isReady() && !main_->getStop()) {
		pr_notice("In scraper (waiting for main thread to be ready)...");
		sleep(1);
	}

	try {
		runScrapers(this);
	} catch (std::runtime_error &e) {
		pr_err("std::runtime_error: %s", e.what());
		main_->doStop();
		goto out;
	}

out:
	pr_notice("Closing scraper...");
}


} /* namespace tgvisd */
