// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules::Enumerate
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MODULES__ENUMERATE__SHELL_HPP
#define TGVISD__MODULES__ENUMERATE__SHELL_HPP

#include <tgvisd/Modules/BaseEnumerate.hpp>

namespace tgvisd::Modules::Enumerate {

class Shell: public tgvisd::Modules::BaseEnumerate
{
public:
	Shell(void);
	virtual ~Shell(void);
	virtual int init(void);
	virtual int shutdown(void);
	virtual mbe_ret_t handleUpdate(tgvisd::Main::Main *main,
				       td_api::updateNewMessage &update_);
};

} /* namespace tgvisd::Modules::Enumerate */

#endif /* #ifndef TGVISD__MODULES__ENUMERATE__SHELL_HPP */
