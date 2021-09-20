// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__SCRAPER_HPP
#define TGVISD__SCRAPER_HPP

#include <tgvisd/Td/Td.hpp>
#include <tgvisd/common.hpp>

#include <tgvisd/Main.hpp>
#include <tgvisd/Scrapers/ChatScraper.hpp>

namespace tgvisd {

class Scraper {
public:
	Scraper(Main *main);
	Scraper(Main *main, std::thread *threadPtr);
	~Scraper(void);
	void run(void);

	inline Main *getMain(void)
	{
		return main_;
	}

	tgvisd::Scrapers::ChatScraper *chatScraper_ = nullptr;

private:
	Main *main_ = nullptr;
};

} /* namespace tgvisd */

#endif /* #ifndef TGVISD__SCRAPER_HPP */
