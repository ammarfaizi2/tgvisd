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
#include <condition_variable>

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
private:
	tgvisd::Td::Td *td_ = nullptr;
	tgvisd::Main::Main *main_ = nullptr;

	void gatherMsg(void);
	void gatherMsgByChatId(int64_t chat_id);
	td_api::object_ptr<td_api::chats> getChats(void);
	td_api::object_ptr<td_api::messages> getChatHistory(int64_t chat_id);

	inline bool stopUpdate(void)
	{
		return main_->stopUpdate();
	}
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


void Worker::run(void)
{
	tgvisd::Main::Main *main = main_;

	while (likely(!main->stopUpdate())) {
		gatherMsg();
		sleep(1);
	}
}


td_api::object_ptr<td_api::chats> Worker::getChats(void)
{
	std::mutex mut;
	bool finished = false;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lock(mut, std::defer_lock);
	td_api::object_ptr<td_api::chats> chats;

	auto callback = [&chats, &cond, &finished](
		td_api::object_ptr<td_api::Object> obj
	) {
		if (obj->get_id() == td_api::error::ID) {
			auto err = td::move_tl_object_as<td_api::error>(obj);
			std::cout
				<< "Error on getChats: "
				<< err->message_
				<< std::endl;
			goto out;
		}

		if (obj->get_id() != td_api::chats::ID) {
			std::cout
				<< "Invalid object returned on getChats"
				<< std::endl;
			goto out;
		}

		chats = td::move_tl_object_as<td_api::chats>(obj);
	out:
		finished = true;
		cond.notify_one();
	};


	td_->send_query(
		td_api::make_object<td_api::getChats>(
			nullptr,
			std::numeric_limits<std::int64_t>::max(),
			0,
			100
		),
		callback
	);


	lock.lock();
	while (!(this->stopUpdate() || finished)) {
		cond.wait_for(lock, 2000ms, [this, &finished](void){
			return this->stopUpdate() || finished;
		});
	}

	if (unlikely(this->stopUpdate()))
		chats = nullptr;

	return chats;
}


td_api::object_ptr<td_api::messages> Worker::getChatHistory(int64_t chat_id)
{
	std::mutex mut;
	bool finished = false;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lock(mut, std::defer_lock);
	td_api::object_ptr<td_api::messages> msgs;

	auto callback = [&msgs, &cond, &finished](
		td_api::object_ptr<td_api::Object> obj
	) {
		if (obj->get_id() == td_api::error::ID) {
			auto err = td::move_tl_object_as<td_api::error>(obj);
			std::cout
				<< "Error on getChatHistory: "
				<< err->message_
				<< std::endl;
			goto out;
		}

		if (obj->get_id() != td_api::messages::ID) {
			std::cout
				<< "Invalid object returned on getChatHistory"
				<< std::endl;
			goto out;
		}

		msgs = td::move_tl_object_as<td_api::messages>(obj);
	out:
		finished = true;
		cond.notify_one();
	};

	int64_t from_message_id = 0;
	int32_t offset = 0;
	int32_t limit = 100;
	bool only_local = false;
	td_->send_query(
		td_api::make_object<td_api::getChatHistory>(
			chat_id,
			from_message_id,
			offset,
			limit,
			only_local
		),
		callback
	);


	lock.lock();
	while (!(this->stopUpdate() || finished)) {
		cond.wait_for(lock, 2000ms, [this, &finished](void){
			return this->stopUpdate() || finished;
		});
	}

	return msgs;
}


void Worker::gatherMsg(void)
{
	td_api::object_ptr<td_api::chats> chats = getChats();

	if (unlikely(this->stopUpdate()))
		return;

	if (!chats) {
		std::cout << "Chat is null on gatherMsg" << std::endl;
		return;
	}

	for (auto chat_id: chats->chat_ids_) {
		if (chat_id > 0)
			continue;

		gatherMsgByChatId(chat_id);
	}

	sleep(1);
}


void Worker::gatherMsgByChatId(int64_t chat_id)
{
	td_api::object_ptr<td_api::messages> msgs = getChatHistory(chat_id);

	if (unlikely(this->stopUpdate()))
		return;

	if (!msgs) {
		std::cout
			<< "Messages is null on gatherMsgByChatId"
			<< std::endl;
		return;
	}

	for (auto &msg: msgs->messages_) {
		if (unlikely(!msg))
			continue;

		auto &content = msg->content_;
		if (content->get_id() != td_api::messageText::ID)
			continue;

		std::string &text = static_cast<td_api::messageText &>(*content).text_->text_;
		printf("%ld = %s\n", (msg->id_ >> 20u), text.c_str());
	}
}


} /* namespace tgvisd::Modules::Preload */
