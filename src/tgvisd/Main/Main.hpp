// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__MAIN_HPP
#define TGVISD__MAIN__MAIN_HPP

#include <tgvisd/Td/Td.hpp>

namespace tgvisd::Main {

class Main {
public:
	Main(uint32_t api_id, const char *api_hash, const char *data_path);
	~Main(void);
	void run(void);
private:
	std::shared_ptr<tgvisd::Td::Td> td_;
};

} /* namespace tgvisd::Main */

#endif /* #ifndef TGVISD__MAIN__MAIN_HPP */
