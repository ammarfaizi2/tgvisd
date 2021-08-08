// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DBCOMMON_HPP
#define TGVISD__DBCOMMON_HPP

#include <string>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <mysqlx/xapi.h>
#include <tgvisd/common.hpp>


inline static void mysql_stmt_create_chk(mysqlx_stmt_t *stmt,
					 mysqlx_session_t *sess)
{
	if (unlikely(!stmt)) {
		char buf[512];
		snprintf(buf, sizeof(buf), "MySQL Statement Error: %s",
			 mysqlx_error_message(sess));
		throw std::runtime_error(std::string(buf));
	}
}


inline static void mysql_exec_chk(int res, mysqlx_stmt_t *stmt)
{
	if (res != RESULT_OK) {
		char buf[512];
		snprintf(buf, sizeof(buf), "MySQL Exec Error: %s",
			 mysqlx_error_message(stmt));
		throw std::runtime_error(std::string(buf));
	}
}


inline static void mysql_result_chk(mysqlx_result_t *res, mysqlx_stmt_t *stmt)
{
	if (unlikely(!res)) {
		char buf[512];
		snprintf(buf, sizeof(buf), "MySQL Result Error: %s",
			 mysqlx_error_message(stmt));
		throw std::runtime_error(std::string(buf));
	}
}


#endif /* #ifndef TGVISD__DBCOMMON_HPP */
