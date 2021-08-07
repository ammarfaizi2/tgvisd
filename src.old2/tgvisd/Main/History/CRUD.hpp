// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::History
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__MAIN__HISTORY__CRUD_HPP
#define TGVISD__MAIN__HISTORY__CRUD_HPP

#include <cstddef>
#include <mysqlx/xapi.h>

#define __must_hold(MUTEX)
#define __releases(MUTEX)
#define __acquires(MUTEX)
#define likely(EXPR)	__builtin_expect((bool)(EXPR), 1)
#define unlikely(EXPR)	__builtin_expect((bool)(EXPR), 0)

#define print_nl(...)		\
do {				\
	printf(__VA_ARGS__);	\
	putchar('\n');		\
} while (0)

#define print_arg(...) print_nl(__VA_ARGS__)

#define pr_debug print_arg
#define pr_notice print_arg
#define pr_warn print_arg
#define pr_err print_arg
#define pr_error print_arg


namespace tgvisd::Main::History {

class CRUD
{
public:
	CRUD(const char *host, uint16_t port, const char *user, const char *pass);
	inline ~CRUD(void) {
		if (sess_)
			mysqlx_session_close(sess_);
	}
	void insertMessage(void);

private:
	char host_[128] = {0};
	uint16_t port = 0;
	char user_[64] = {0};
	char pass_[64] = {0};

	mysqlx_session_t *sess_ = nullptr;
	mysqlx_stmt_t *crud_ = nullptr;
	mysqlx_result_t *res_ = nullptr;
	mysqlx_row_t *row_ = nullptr;
	mysqlx_schema_t *db_ = nullptr;
	mysqlx_table_t *table_ = nullptr;
	mysqlx_error_t *error_ = nullptr;
};

} /* namespace tgvisd::Main::History */

#endif /* #ifndef TGVISD__MAIN__HISTORY__CRUD_HPP */
