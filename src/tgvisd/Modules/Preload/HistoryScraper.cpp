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
#include <mutex>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <condition_variable>

#include <tgvisd/DB.hpp>

#include "HistoryScraper.hpp"

namespace tgvisd::Modules::Preload {

using namespace std::chrono_literals;

HistoryScraper::HistoryScraper(void)
{
}

class Worker
{
public:
	Worker(tgvisd::Main::Main *main);
	void run(void);
	inline bool stopUpdate(void)
	{
		return main_->stopUpdate();
	}

	inline tgvisd::Td::Td *getTd(void)
	{
		return td_;
	}


	// void gatherMsg(void);
	// void gatherMsgByChatId(int64_t chat_id);
	// td_api::object_ptr<td_api::chat> getChat(int64_t chat_id);
	// td_api::object_ptr<td_api::chats> getChats(void);
	// td_api::object_ptr<td_api::messages> getChatHistory(int64_t chat_id);

	void gatherChatEventLog(int64_t chat_id);
	void processChatEventLog(td_api::object_ptr<td_api::chatEvents> events);
private:
	tgvisd::Td::Td *td_ = nullptr;
	tgvisd::Main::Main *main_ = nullptr;

};


mbp_ret_t HistoryScraper::initPreload(tgvisd::Main::Main *main)
{
	main->incRef();
	std::thread worker = std::thread([main](void){
		Worker *wrk = new Worker(main);
		wrk->run();
		delete wrk;
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


Worker::Worker(tgvisd::Main::Main *main):
	main_(main)
{
	td_ = main->getTd();
	assert(td_);
}


// td_api::object_ptr<td_api::chats> Worker::getChats(void)
// {
// 	return td_->send_query_sync<td_api::getChats, td_api::chats>(
// 		td_api::make_object<td_api::getChats>(
// 			nullptr,
// 			std::numeric_limits<std::int64_t>::max(),
// 			0,
// 			100
// 		)
// 	);
// }


// td_api::object_ptr<td_api::messages> Worker::getChatHistory(int64_t chat_id)
// {
// 	int64_t from_message_id = 0;
// 	int32_t offset = 0;
// 	int32_t limit = 100;
// 	bool only_local = false;
// 	return td_->send_query_sync<td_api::getChatHistory, td_api::messages>(
// 		td_api::make_object<td_api::getChatHistory>(
// 			chat_id,
// 			from_message_id,
// 			offset,
// 			limit,
// 			only_local
// 		)
// 	);
// }


// td_api::object_ptr<td_api::chat> Worker::getChat(int64_t chat_id)
// {
// 	return td_->send_query_sync<td_api::getChat, td_api::chat>(
// 		td_api::make_object<td_api::getChat>(chat_id)
// 	);
// }


// void Worker::gatherMsg(void)
// {
// 	td_api::object_ptr<td_api::chats> chats = getChats();

// 	if (unlikely(this->stopUpdate()))
// 		return;

// 	if (!chats) {
// 		std::cout << "Chat is null on gatherMsg" << std::endl;
// 		return;
// 	}

// 	for (auto chat_id: chats->chat_ids_) {
// 		if (chat_id > 0)
// 			continue;

// 		gatherMsgByChatId(chat_id);
// 	}

// 	sleep(1);
// }


// void Worker::gatherMsgByChatId(int64_t chat_id)
// {
// 	td_api::object_ptr<td_api::messages> msgs = getChatHistory(chat_id);

// 	if (unlikely(this->stopUpdate()))
// 		return;

// 	if (!msgs) {
// 		std::cout
// 			<< "Messages is null on gatherMsgByChatId"
// 			<< std::endl;
// 		return;
// 	}

// 	if (msgs->total_count_ < 5)
// 		return;

// 	td_api::object_ptr<td_api::chat> chat = getChat(chat_id);


// 	for (auto &msg: msgs->messages_) {

// 		if (unlikely(!msg))
// 			continue;

// 		auto &content = msg->content_;
// 		if (content->get_id() != td_api::messageText::ID)
// 			continue;

// 		if (chat->title_ != "GNU/Weeb")
// 			continue;

// 		std::string &text = static_cast<td_api::messageText &>(*content).text_->text_;
// 		printf("[chat_id: %ld] [title: %s] [msg_id: %ld]: \"%s\"\n",
// 			chat->id_,
// 			chat->title_.c_str(),
// 			(msg->id_ >> 20u),
// 			text.c_str());
// 	}
// }



void Worker::run(void)
{
	// tgvisd::DB db("127.0.0.1", 0, "memmove");
	// tgvisd::Main::Main *main = main_;

	// while (likely(!main->stopUpdate())) {
	// 	// gatherMsg();
	// 	sleep(1);
	// }



	sleep(2);
	// gatherChatEventLog(-1001226735471); // Private Cloud
	// gatherChatEventLog(-1001483770714); // GNU/Weeb
}


void Worker::gatherChatEventLog(int64_t chat_id)
{
	std::vector<int32_t> user_ids;
	printf("In gatherChatEventLog\n");
	td_->send_query(
		td_api::make_object<td_api::getChatEventLog>(
			chat_id,
			"",
			0,
			100,
			nullptr,
			std::move(user_ids)
		),
		[this](td_api::object_ptr<td_api::Object> obj){
			if (obj->get_id() == td_api::error::ID) {
				auto err = td::move_tl_object_as<td_api::error>(obj);
				printf("Error on gatherChatEventLog: %s\n",
				       err->message_.c_str());
				return;
			}

			if (obj->get_id() != td_api::chatEvents::ID) {
				printf("Wrong object on gatherChatEventLog\n");
				return;
			}

			this->processChatEventLog(
				td::move_tl_object_as<td_api::chatEvents>(obj)
			);
		}
	);
}


void Worker::processChatEventLog(td_api::object_ptr<td_api::chatEvents> events)
{
	for (auto &ev: events->events_) {

		if (ev->action_->get_id() != td_api::chatEventMessageDeleted::ID)
			continue;

		auto &msg = static_cast<td_api::chatEventMessageDeleted &>(*ev->action_).message_;
		std::cout << to_string(msg) << std::endl;
	}
}


} /* namespace tgvisd::Modules::Preload */
