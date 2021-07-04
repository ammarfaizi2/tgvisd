// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__BASE_HPP
#define TGVISD__MODULES__BASE_HPP

#include <cassert>
#include <tgvisd/Main/Main.hpp>

typedef enum module_res_t_ {
	MRT_SKIP_MODULE
} module_res_t;

namespace tgvisd::Modules {

class Base
{
public:
	Base(void)
	{
	}

	virtual ~Base(void)
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

	virtual module_res_t handleUpdate(tgvisd::Main::Main *main,
					  td_api::updateNewMessage &update_)
	{
		return MRT_SKIP_MODULE;
	}

protected:
	tgvisd::Main::Main *main_ = nullptr;
};

} /* namespace tgvisd::Modules */

#endif /* #ifndef TGVISD__MODULES__BASE_HPP */
