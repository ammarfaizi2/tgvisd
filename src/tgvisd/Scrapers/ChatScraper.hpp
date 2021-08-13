// SPDX-License-Identifier: GPL-2.0
/*
 * @author Alviro Iskandar Setiawan <alviro@gnuweeb.org>
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Alviro Iskandar Setiawan <alviro@gnuweeb.org>
 */

#ifndef TGVISD__SCRAPERS__CHATSCRAPER_HPP
#define TGVISD__SCRAPERS__CHATSCRAPER_HPP

#include <tgvisd/DB.hpp>
#include <tgvisd/Td/Td.hpp>
#include <tgvisd/common.hpp>

namespace tgvisd {

class Scraper;

} /* namespace tgvisd */

namespace tgvisd::Scrapers {

class ChatScraper {
public:
	ChatScraper(tgvisd::Scraper *scraper);
	~ChatScraper(void);
	void run(void);

	td_api::object_ptr<td_api::chats> getChat(
		td_api::object_ptr<td_api::ChatList> &&chat_list,
		int64_t offset_order,
		int64_t offset_chat_id,
		int32_t limit
	);
private:
	tgvisd::DB *db_ = nullptr;
	tgvisd::Td::Td *td_ = nullptr;
	tgvisd::Scraper *scraper_ = nullptr;
};

} /* namespace tgvisd::Scrapers */

#include <tgvisd/Scraper.hpp>

#endif /* #ifndef TGVISD__SCRAPERS__CHATSCRAPER_HPP */
