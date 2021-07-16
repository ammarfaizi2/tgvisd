// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TAKE_MODULE_TABLE_DEFINITION

#include <tgvisd/Modules/BasePreload.hpp>
#include <tgvisd/Modules/Preload/HistoryScraper.hpp>

using tgvisd::Modules::BasePreload;

#else /* #ifdef TAKE_MODULE_TABLE_DEFINITION */

BasePreload *moduleTablePreload_[2] = {
	new tgvisd::Modules::Preload::HistoryScraper(),
	nullptr
};

#endif
