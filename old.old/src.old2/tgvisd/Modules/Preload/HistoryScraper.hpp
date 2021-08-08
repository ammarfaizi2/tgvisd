// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER_HPP
#define TGVISD__MODULES__PRELOAD__HISTORYSCRAPER_HPP

#include <tgvisd/Modules/BasePreload.hpp>

namespace tgvisd::Modules::Preload {


class HistoryScraper: public BasePreload
{
public:
	HistoryScraper(void);
	virtual mbp_ret_t initPreload(tgvisd::Main::Main *main);
};


} /* namespace tgvisd::Modules::Preload */

#endif /* #ifndef TGVISD__MODULES__PRELOAD__HISTORYSCRAPER_HPP */
