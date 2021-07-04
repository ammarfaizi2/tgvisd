// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules::Shell
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstdio>
#include "Shell.hpp"

namespace tgvisd::Modules::Shell {

int Shell::init(void)
{
	printf("Starting up shell module...\n");
	return 0;
}


int Shell::shutdown(void)
{
	printf("Shutting down shell module...\n");
	return 0;
}


module_res_t Shell::handleUpdate(tgvisd::Main::Main *main,
				 td_api::updateNewMessage &update_)
{
	printf("Shell module here :)\n");
	return MRT_SKIP_MODULE;
}


} /* namespace tgvisd::Modules::Shell */
