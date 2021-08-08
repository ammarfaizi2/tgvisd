// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DB_HPP
#define TGVISD__DB_HPP

#include <mysqlx/xapi.h>
#include <tgvisd/common.hpp>

namespace tgvisd {

class DB {
public:
	DB(void);
	~DB(void);
	DB(const char *host, uint16_t port, const char *user, const char *pass,
	   const char *database);
	void connect(void);
	void close(void);

	inline mysqlx_session_t *getSess(void)
	{
		return sess_;
	}

private:
	uint16_t port_;
	char host_[128];
	char user_[128];
	char pass_[128];
	char database_[128];
	mysqlx_session_t *sess_ = nullptr;
};

} /* namespace tgvisd */

#endif /* #ifndef TGVISD__DB_HPP */
