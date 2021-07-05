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
	tgvisd::Modules::Base **modTbl = moduleTable_, *mod;

	while ((mod = *modTbl++)) {
		mod->init();
	}
}


void Module::shutdownModule()
{
	tgvisd::Modules::Base **modTbl = moduleTable_, *mod;

	while ((mod = *modTbl)) {
		mod->shutdown();
		delete mod;
		*modTbl++ = nullptr;
	}
}


void Module::enumerate(Main *main, td_api::updateNewMessage &update)
{
	size_t i = 0;
	module_res_t ret;
	tgvisd::Modules::Base **modTbl = moduleTable_, *mod;

	while ((mod = *modTbl++)) {
		i++;
		ret = mod->handleUpdate(main, update);
		switch (ret) {
		case MRT_STOP:
			goto out;
		case MRT_CONTINUE:
			if (!update.get_id()) {
				printf("PANIC!!!\n");
				printf("Fatal: handleUpdate() moved the `update` pointer but returned MRT_CONTINUE\n");
				printf("File: %s:%d\n", __FILE__, __LINE__);
				printf("Iterator count: %zu\n", i);
				abort();
			}
			break;
		}
	}

out:
	return;
}

} /* namespace tgvisd::Main */