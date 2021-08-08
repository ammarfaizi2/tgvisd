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
	~PreparedStatement(void);

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
		if (res_) {
			mysqlx_free(res_);
			res_ = nullptr;
		}
		pr_debug("Executing...");
		res_ = mysqlx_execute(stmt_);
		RESULT_CHECK(res_, stmt_);
		return res_;
	}


	template<typename... Args>
	inline mysqlx_result_t *execute(Args... args)
	{
		bind(args...);
		return execute();
	}

	inline mysqlx_row_t *fetch(void)
	{
		return mysqlx_row_fetch_one(res_);
	}

	inline mysqlx_stmt_t *getStmt(void)
	{
		return stmt_;
	}

	inline mysqlx_result_t *getRes(void)
	{
		return res_;
	}
private:
	mysqlx_session_t *sess_ = nullptr;
	mysqlx_result_t *res_ = nullptr;
	mysqlx_stmt_t *stmt_ = nullptr;
};

} /* namespace tgvisd::DBt */

#endif /* #ifndef TGVISD__DBT__PREPAREDSTATEMENT_HPP */
