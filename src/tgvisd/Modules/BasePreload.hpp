// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__BASEPRELOAD_HPP
#define TGVISD__MODULES__BASEPRELOAD_HPP

#include <tgvisd/Main/Main.hpp>

typedef enum mbp_ret_t_ {
	MBP_BREAK,
	MBP_CONTINUE
} mbp_ret_t;

namespace tgvisd::Modules {

class BasePreload
{
public:
	BasePreload(void)
	{
	}

	virtual ~BasePreload(void)
	{
	}

	virtual mbp_ret_t initPreload(tgvisd::Main::Main *main)
	{
		return MBP_CONTINUE;
	}
};

} /* namespace tgvisd::Modules */

#endif /* #ifndef TGVISD__MODULES__BASEPRELOAD_HPP */
