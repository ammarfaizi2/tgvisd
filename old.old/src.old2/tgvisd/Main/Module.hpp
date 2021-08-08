// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__MODULE_HPP
#define TGVISD__MAIN__MODULE_HPP

#include <tgvisd/Td/Td.hpp>

#include "ModuleTablePreload.hpp"
#include "ModuleTableEnumerate.hpp"

namespace tgvisd::Main {

class Main;

class Module
{
public:
	Module(void);
	~Module(void);
	void initPreloadModules(Main *main);
	void dispatchEnumerate(Main *main, td_api::updateNewMessage &update_);

private:
	void shutdownPreloadModules(void);
	void shutdownEnumerateModules(void);


#define TAKE_MODULE_TABLE_DEFINITION
#include "ModuleTablePreload.hpp"
#include "ModuleTableEnumerate.hpp"
#undef TAKE_MODULE_TABLE_DEFINITION

};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__MODULE_HPP */
