// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN_HPP
#define TGVISD__MAIN_HPP

#include <tgvisd/Td/Td.hpp>
#include <tgvisd/common.hpp>

namespace tgvisd {

extern volatile bool stopEventLoop;

class Main {
public:
	Main(uint32_t api_id, const char *api_hash, const char *data_path);
	~Main(void);
	int run(void);

	inline bool getStop(void)
	{
		return stopEventLoop;
	}

	inline bool isReady(void)
	{
		return isReady_;
	}

private:
	tgvisd::Td::Td td_;
	volatile bool isReady_ = false;
	std::thread *loggerThread_ = nullptr;
	std::thread *scraperThread_ = nullptr;
};

} /* namespace tgvisd */

#endif /* #ifndef TGVISD__MAIN_HPP */
