// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__BASEENUMERATE_HPP
#define TGVISD__MODULES__BASEENUMERATE_HPP

#include <tgvisd/Main/Main.hpp>

typedef enum mbe_ret_t_ {
	MBE_BREAK,
	MBE_CONTINUE
} mbe_ret_t;

namespace tgvisd::Modules {

class BaseEnumerate
{
public:
	BaseEnumerate(void)
	{
	}

	virtual ~BaseEnumerate(void)
	{
	}

	virtual int init(void)
	{
		return 0;
	}

	virtual int shutdown(void)
	{
		return 0;
	}

	virtual mbe_ret_t handleUpdate(tgvisd::Main::Main *main,
				       td_api::updateNewMessage &update)
	{
		return MBE_CONTINUE;
	}
};

} /* namespace tgvisd::Modules */

#endif /* #ifndef TGVISD__MODULES__BASEENUMERATE_HPP */
