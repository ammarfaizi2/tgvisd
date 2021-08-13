// SPDX-License-Identifier: GPL-2.0
/*
 * @author Alviro Iskandar Setiawan <alviro@gnuweeb.org>
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Alviro Iskandar Setiawan <alviro@gnuweeb.org>
 */

#include <limits>
#include <tgvisd/Scrapers/ChatScraper.hpp>

namespace tgvisd::Scrapers {


ChatScraper::ChatScraper(tgvisd::Scraper *scraper):
	scraper_(scraper)
{
	pr_debug("Initializing chat scraper...");
	db_ = DB::create_conn_from_env();
	db_->connect();
	td_ = scraper_->getMain()->getTd();
}


ChatScraper::~ChatScraper(void)
{
	if (db_) {
		delete db_;
		db_ = nullptr;
	}
}


void ChatScraper::run(void)
{
	/*
	 * This function retrieves the list of chat_ids.
	 */
	pr_debug("ChatScraper: Getting chatList...");
	auto chats = getChat(
		nullptr,
		std::numeric_limits<std::int64_t>::max(),
		0,
		300
	);
	pr_debug("ChatScraper: Got %d chat ID(s)", chats->total_count_);
}


td_api::object_ptr<td_api::chats> ChatScraper::getChat(
		td_api::object_ptr<td_api::ChatList> &&chat_list,
		int64_t offset_order,
		int64_t offset_chat_id,
		int32_t limit
	)
{
	return td_->send_query_sync<td_api::getChats, td_api::chats>(
		td_api::make_object<td_api::getChats>(
			std::move(chat_list),
			offset_order,
			offset_chat_id,
			limit
		)
	);
}


} /* namespace tgvisd::Scrapers */
