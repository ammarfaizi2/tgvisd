// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TAKE_MODULE_TABLE_DEFINITION

#include <tgvisd/Modules/BaseEnumerate.hpp>
#include <tgvisd/Modules/BasePreload.hpp>
#include <tgvisd/Modules/Enumerate/Shell.hpp>

using tgvisd::Modules::BaseEnumerate;

#else /* #ifdef TAKE_MODULE_TABLE_DEFINITION */

BaseEnumerate *moduleTableEnumerate_[2] = {
	new tgvisd::Modules::Enumerate::Shell(),
	nullptr
};

#endif
