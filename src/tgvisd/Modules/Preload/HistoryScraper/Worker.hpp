// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload::HistoryScraper
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__WORKER_HPP
#define TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__WORKER_HPP

#include <tgvisd/Modules/BasePreload.hpp>

namespace tgvisd::Modules::Preload::HistoryScraper {


class Worker
{
public:
	Worker(tgvisd::Main::Main *main);
	void run(void);
private:
	tgvisd::Main::Main *main_ = nullptr;

	void gatherMsg(tgvisd::Main::Main *main);
	void gatherMsgByChatId(tgvisd::Main::Main *main, int64_t chat_id);
};


} /* namespace tgvisd::Modules::Preload::HistoryScraper */

#endif /* #ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__WORKER_HPP */
