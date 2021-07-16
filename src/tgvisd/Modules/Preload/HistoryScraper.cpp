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
	while (likely(!main->stopUpdate())) {
		gatherData(main);
		sleep(3);
	}
}


void HistoryScraper::gatherData(tgvisd::Main::Main *main)
{
	std::mutex lk;
	int64_t chat_id = -1001226735471;
	int64_t from_message_id = 1;
	int32_t offset = -99;
	int32_t limit = 100;
	bool only_local = false;
	tgvisd::Td::Td *td = main->getTd();
	td_api::object_ptr<td_api::messages> pMsg;


	printf("Getting chat history...\n");
	auto req = td_api::make_object<td_api::getChatHistory>(
			chat_id, from_message_id, offset, limit, only_local);

	lk.lock();
	td->send_query(std::move(req), [this, &lk, &pMsg](auto up){
		td_api::object_ptr<td_api::Object> &obj = up;

		if (obj->get_id() == td_api::error::ID) {
			auto err = td::move_tl_object_as<td_api::error>(obj);
			std::cout
				<< "Error on handleMessageVector: "
				<< err->message_
				<< std::endl;
			goto out;
		}

		if (obj->get_id() != td_api::messages::ID) {
			std::cout
				<< "Invalid object returned on gatherData"
				<< std::endl;
			goto out;
		}

		pMsg = td::move_tl_object_as<td_api::messages>(obj);
	out:
		std::cout << "Unlocked!" << std::endl;
		lk.unlock();
	});
	lk.lock();
	handleMessageVector(pMsg);
	lk.unlock();
}


void HistoryScraper::handleMessageVector(td_api::object_ptr<td_api::messages> &pMsg)
{
	if (!pMsg) {
		printf("pMsg is NULL\n");
		return;
	}

	auto &messages = pMsg->messages_;
	int32_t count = pMsg->total_count_;

	printf("count = %d\n", count);
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



} /* namespace tgvisd::Modules::Preload */
