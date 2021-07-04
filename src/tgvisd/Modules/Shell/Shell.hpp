// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules::Shell
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__ShELL__SHELL_HPP
#define TGVISD__MODULES__ShELL__SHELL_HPP

#include <tgvisd/Modules/Base.hpp>

namespace tgvisd::Modules::Shell {

using tgvisd::Modules::Base;

class Shell: public Base
{
public:
	virtual int init(void);
	virtual int shutdown(void);
	virtual module_res_t handleUpdate(tgvisd::Main::Main *main,
					  td_api::updateNewMessage &update_);
};

} /* namespace tgvisd::Modules::Shell */

#endif /* #ifndef TGVISD__MODULES__ShELL__SHELL_HPP */
