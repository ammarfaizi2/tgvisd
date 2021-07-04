// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Modules::Shell
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cctype>
#include <cstdio>
#include <cstring>
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


static void doShellExec(tgvisd::Main::Main *main,
			td_api::updateNewMessage update, const char *cmd,
			size_t cmdLen);


module_res_t Shell::handleUpdate(tgvisd::Main::Main *main,
				 td_api::updateNewMessage &update)
{
	auto &update_ = update;
	auto &message = update_.message_;
	auto &content = *(message->content_);

	if (message->content_->get_id() != td_api::messageText::ID)
		return MRT_CONTINUE;


	std::string &text = static_cast<td_api::messageText &>(content).text_->text_;
	char c;
	const char *cstr = text.c_str();

	if (text.length() < 4)
		return MRT_CONTINUE;

	c = *cstr++;
	if (!(c == '!' || c == '/' || c == '~' || c == '.'))
		return MRT_CONTINUE;

	if (!(cstr[0] == 's' && cstr[1] == 'h'))
		return MRT_CONTINUE;

	if (!isspace((unsigned)cstr[2]))
		return MRT_CONTINUE;

	{
		cstr += 3;
		main->incRef();
		std::thread shellThread(doShellExec, main, std::move(update),
					cstr, text.c_str());
		shellThread.detach();
	}

	return MRT_STOP;
}


static void doShellExec(tgvisd::Main::Main *main,
			td_api::updateNewMessage update, const char *cmd,
			size_t cmdLen)
{
	/* TODO: Handle shell command. */
	printf("shell cmd = \"%s\"\n", cmd);
	main->decRef();
}


} /* namespace tgvisd::Modules::Shell */
