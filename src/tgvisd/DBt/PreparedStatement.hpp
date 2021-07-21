// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::DBt
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DBT__PREPAREDSTATEMENT_HPP
#define TGVISD__DBT__PREPAREDSTATEMENT_HPP

#include <mysqlx/xapi.h>
#include <tgvisd/common.hpp>
#include <tgvisd/DBCommon.hpp>

namespace tgvisd::DBt {

class PreparedStatement
{
public:
	inline PreparedStatement(mysqlx_session_t *sess, const char *query,
				 uint32_t length = MYSQLX_NULL_TERMINATED):
		sess_(sess)
	{
		stmt_ = mysqlx_sql_new(sess, query, length);
		CRUD_CHECK(stmt_, sess);
		(void)sess_;
	}

	template<typename... Args>
	inline int bind(Args... args)
	{
		pr_debug("Binding value...");
		int ret = mysqlx_stmt_bind(stmt_, args...);
		IS_OK(ret, stmt_);
		return ret;
	}


	inline mysqlx_result_t *execute(void)
	{
		mysqlx_result_t *res;
		res = mysqlx_execute(stmt_);
		RESULT_CHECK(res, stmt_);
		return res;
	}


	template<typename... Args>
	inline mysqlx_result_t *execute(Args... args)
	{
		bind(args...);
		return execute();
	}

private:
	mysqlx_session_t *sess_ = nullptr;
	mysqlx_stmt_t *stmt_ = nullptr;
};

} /* namespace tgvisd::DBt */

#endif /* #ifndef TGVISD__DBT__PREPAREDSTATEMENT_HPP */
