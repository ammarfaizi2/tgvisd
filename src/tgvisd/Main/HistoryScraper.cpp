// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <iostream>

#include "Main.hpp"
#include "HistoryScraper.hpp"

namespace tgvisd::Main {


HistoryScraper::HistoryScraper(Main *main):
	main_(main)
{
}


HistoryScraper::~HistoryScraper(void)
{
	stopEventLoop_ = true;
	if (thread_) {
		thread_->join();
		delete thread_;
	}
}


void HistoryScraper::spawn(void)
{
	thread_ = new std::thread([this](void){
		this->run();
	});
}


void HistoryScraper::run(void)
{
#if defined(__linux__)
	while (this->thread_ == nullptr);

	pthread_t pt = this->thread_->native_handle();
	pthread_setname_np(pt, "history-scraper");
#endif
	while (likely(!stopEventLoop_)) {
		gatherHistory();
		sleep(10);
	}
}


void HistoryScraper::gatherHistory(void)
{
	int64_t chat_id = -1001226735471;
	int64_t from_message_id = 1;
	int32_t offset = -99;
	int32_t limit = 100;
	bool only_local = false;
	tgvisd::Td::Td *td = main_->getTd();

	auto req = td_api::make_object<td_api::getChatHistory>(
			chat_id, from_message_id, offset, limit, only_local);

	td->send_query(std::move(req), [this](auto up){
		this->handleMessageVector(
			td::move_tl_object_as<td_api::messages>(up)
		);
	});
}


void HistoryScraper::handleMessageVector(td_api::object_ptr<td_api::messages> pMsg)
{
	auto &messages = pMsg->messages_;
	int32_t count = pMsg->total_count_;

	for (int32_t i = 0; i < count; i++) {
		auto &message = messages[i];
		if (!message)
			continue;

		auto &content = message->content_;
		if (!content)
			continue;

		if (content->get_id() != td_api::messageText::ID)
			continue;

		std::string &text = static_cast<td_api::messageText &>(*content).text_->text_;
		printf("%ld = %s\n", (message->id_ >> 20u), text.c_str());
	}
}


} /* namespace tgvisd::Main */
