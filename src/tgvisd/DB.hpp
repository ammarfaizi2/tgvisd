// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DB_HPP
#define TGVISD__DB_HPP

#include "common.hpp"
#include <tgvisd/DBCommon.hpp>
#include <tgvisd/DBt/PreparedStatement.hpp>

namespace tgvisd {

class DB
{
public:
	DB(void);
	DB(const char *host, uint16_t port, const char *user, const char *pass,
	   const char *database);
	bool connect(void);

	inline std::unique_ptr<tgvisd::DBt::PreparedStatement>
	prepare(const char *query)
	{
		return std::make_unique<tgvisd::DBt::PreparedStatement>(sess_,
									query);
	}
private:
	char host_[128] = {0};
	uint16_t port_ = 3306;
	char user_[64] = {0};
	char pass_[64] = {0};
	char database_[64] = {0};
	mysqlx_session_t *sess_ = nullptr;
};

} /* namespace tgvisd */

#endif /* #ifndef TGVISD__DB_HPP */
