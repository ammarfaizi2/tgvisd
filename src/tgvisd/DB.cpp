// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstring>
#include <tgvisd/DB.hpp>


namespace tgvisd {

DB::DB(void)
{
}


DB::DB(const char *host, uint16_t port, const char *user, const char *pass,
       const char *database):
{
	strncpy(host_, host, sizeof(host_) - 1);
	strncpy(user_, user, sizeof(user_) - 1);
	strncpy(pass_, pass, sizeof(pass_) - 1);
	strncpy(database_, database, sizeof(database_) - 1);
	host_[sizeof(host_) - 1] = '\0';
	user_[sizeof(user_) - 1] = '\0';
	pass_[sizeof(pass_) - 1] = '\0';
	database_[sizeof(database_) - 1] = '\0';

	if (port == 0)
		port_ = 33060;
}


void DB::connect(void)
{
	mysqlx_session_t *sess;
	mysqlx_error_t *err = nullptr;

	pr_debug("[DB] Connecting to %s:%d...", host_, port_);
	sess = mysqlx_get_session(host_, port_, user_, pass_, database_, &err);
	if (!sess) {
		char buf[512];
		snprintf(buf, sizeof(buf), "MySQL Error: %s (code: %d)",
			 mysqlx_error_message(err), mysqlx_error_num(err));
		mysqlx_free(err);
		throw std::runtime_error(std::string(buf));
	}
	sess_ = sess;
	pr_debug("[DB] Connection established (%s:%d)", host_, port_);
}


DB::~DB(void)
{
	if (sess_) {
		mysqlx_session_close(sess_);
		sess_ = nullptr;
	}
}

} /* namespace tgvisd */
