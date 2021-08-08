// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::DBFunc
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DBFUNC__DBSTATEMENT_HPP
#define TGVISD__DBFUNC__DBSTATEMENT_HPP

#include <mysqlx/xapi.h>
#include <tgvisd/common.hpp>
#include <tgvisd/DBCommon.hpp>

namespace tgvisd::DBFunc {

class DBStatement {
public:
	~DBStatement(void) = default;

	inline DBStatement(mysqlx_session_t *sess, const char *query,
			   uint32_t length = MYSQLX_NULL_TERMINATED)
	{
		stmt_ = mysqlx_sql_new(sess, query, length);
		mysql_stmt_create_chk(stmt_, sess);
	}


	inline mysqlx_result_t *execute(void)
	{
		if (res_) {
			mysqlx_free(res_);
			res_ = nullptr;
		}
		res_ = mysqlx_execute(stmt_);
		mysql_result_chk(res_, stmt_);
		return res_;
	}


	template<typename... Args>
	inline int bind(Args... args)
	{
		int ret = mysqlx_stmt_bind(stmt_, args...);
		mysql_exec_chk(ret, stmt_);
		return ret;
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
	mysqlx_result_t *res_ = nullptr;
	mysqlx_stmt_t *stmt_ = nullptr;
};

} /* namespace tgvisd::DBFunc */

#endif /* #ifndef TGVISD__DBFUNC__DBSTATEMENT_HPP */
