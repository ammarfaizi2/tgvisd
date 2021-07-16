// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload::HistoryScraper
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__MAIN_HPP
#define TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__MAIN_HPP

#include <tgvisd/Modules/BasePreload.hpp>

namespace tgvisd::Modules::Preload::HistoryScraper {


class Main: public BasePreload
{
public:
	Main(void);
	virtual mbp_ret_t initPreload(tgvisd::Main::Main *main);
private:
	void run(tgvisd::Main::Main *main);
};


} /* namespace tgvisd::Modules::Preload::HistoryScraper */

#endif /* #ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER__MAIN_HPP */
