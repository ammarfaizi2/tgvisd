// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::History
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */


#include "CRUD.hpp"
#include <cstdio>
#include <cstring>

namespace tgvisd::Main::History {

CRUD::CRUD(const char *host, uint16_t port, const char *user, const char *pass)
{
	char url[sizeof(host_) + sizeof(user_) + sizeof(pass_) + 64];

	strncpy(host_, host, sizeof(host_) - 1);
	strncpy(user_, user, sizeof(user_) - 1);
	strncpy(pass_, pass, sizeof(pass_) - 1);
	host_[sizeof(host_) - 1] = '\0';
	user_[sizeof(user_) - 1] = '\0';
	pass_[sizeof(pass_) - 1] = '\0';

	snprintf(url, sizeof(url), "mysqlx://%s:%s@%s", user_, pass_, host_);

	pr_debug("Connecting to mysql...");
	sess_ = mysqlx_get_session_from_url(url, &error_);
	if (!sess_) {
		printf("Error! %s. Error Code: %d\n",
			mysqlx_error_message(error_),
			mysqlx_error_num(error_));
		mysqlx_free(error_);
	}
}

void CRUD::insertMessage(void)
{
	char buffer[1024];
	size_t len = sizeof(buffer);

	pr_debug("Executing query...");

	res_ = mysqlx_sql(sess_, "show variables like 'version'", MYSQLX_NULL_TERMINATED);
	row_ = mysqlx_row_fetch_one(res_);
	mysqlx_get_bytes(row_, 1, 0, buffer, &len);
	mysqlx_free(res_);
	pr_debug("buffer = \"%s\"", buffer);
}

} /* namespace tgvisd::Main::History */
