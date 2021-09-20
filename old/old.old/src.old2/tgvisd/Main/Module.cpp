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
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;
	while ((mod = *modTbl++)) {
		mod->init();
	}
}


Module::~Module(void)
{
	shutdownPreloadModules();
	shutdownEnumerateModules();
}


void Module::initPreloadModules(Main *main)
{
	mbp_ret_t ret;
	BasePreload **modTbl = moduleTablePreload_, *mod;

	while ((mod = *modTbl++)) {
		ret = mod->initPreload(main);
		switch (ret) {
		case MBP_BREAK:
			return;
		case MBP_CONTINUE:
			break;
		}
	}
}


void Module::dispatchEnumerate(Main *main, td_api::updateNewMessage &update)
{
	mbe_ret_t ret;
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;

	while ((mod = *modTbl++)) {
		ret = mod->handleUpdate(main, update);
		switch (ret) {
		case MBE_BREAK:
			return;
		case MBE_CONTINUE:
			break;
		}
	}
}


void Module::shutdownPreloadModules(void)
{
	BasePreload **modTbl = moduleTablePreload_, *mod;

	while ((mod = *modTbl)) {
		delete mod;
		*modTbl++ = nullptr;
	}
}


void Module::shutdownEnumerateModules()
{
	BaseEnumerate **modTbl = moduleTableEnumerate_, *mod;

	while ((mod = *modTbl)) {
		mod->shutdown();
		delete mod;
		*modTbl++ = nullptr;
	}
}


} /* namespace tgvisd::Main */
