// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include "Module.hpp"


namespace tgvisd::Main {


Module::Module(void)
{
	initModule();
}


Module::~Module(void)
{
	shutdownModule();
}


void Module::initModule(void)
{
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;

	while ((mod = *modTbl++)) {
		mod->init();
	}
}


void Module::shutdownModule()
{
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;

	while ((mod = *modTbl)) {
		mod->shutdown();
		delete mod;
		*modTbl++ = nullptr;
	}
}


void Module::enumerate(Main *main, td_api::updateNewMessage &update)
{
	mbe_ret_t ret;
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;

	while ((mod = *modTbl++)) {
		ret = mod->handleUpdate(main, update);
		switch (ret) {
		case MBE_BREAK:
			goto out;
		case MBE_CONTINUE:
			break;
		}
	}

out:
	return;
}

} /* namespace tgvisd::Main */
