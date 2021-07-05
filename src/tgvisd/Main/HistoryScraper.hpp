// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__HISTORYSCRAPER_HPP
#define TGVISD__MAIN__HISTORYSCRAPER_HPP

#include <thread>
#include <tgvisd/Td/Td.hpp>
#include <tgvisd/Main/History/CRUD.hpp>

#define __must_hold(MUTEX)
#define __releases(MUTEX)
#define __acquires(MUTEX)
#define likely(EXPR)	__builtin_expect((bool)(EXPR), 1)
#define unlikely(EXPR)	__builtin_expect((bool)(EXPR), 0)

using tgvisd::Main::History::CRUD;

namespace tgvisd::Main {

class Main;

class HistoryScraper
{

public:
	HistoryScraper(Main *main);
	~HistoryScraper(void);
	void spawn(void);

private:
	Main *main_= nullptr;
	std::thread *thread_ = nullptr;
	bool stopEventLoop_ = false;
	CRUD *crud_ = nullptr;

	void run(void);
	void gatherHistory(void);
	void handleMessageVector(td_api::object_ptr<td_api::messages> pMsg);
};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__HISTORYSCRAPER_HPP */
