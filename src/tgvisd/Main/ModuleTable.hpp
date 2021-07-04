// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */


#ifndef TAKE_MODULE_TABLE_DEFINITION

#include <tgvisd/Modules/Base.hpp>
#include <tgvisd/Modules/Shell/Shell.hpp>

#else /* #ifdef TAKE_MODULE_TABLE_DEFINITION */


tgvisd::Modules::Base *moduleTable_[2] = {
	new Modules::Shell::Shell(),
	nullptr
};

#endif
