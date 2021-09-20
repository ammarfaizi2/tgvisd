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
       const char *database)
{
	/*
	 * We save the credentials for reconnect purpose.
	 */
	strncpy(host_, host, sizeof(host_) - 1);
	strncpy(user_, user, sizeof(user_) - 1);
	strncpy(pass_, pass, sizeof(pass_) - 1);
	strncpy(database_, database, sizeof(database_) - 1);
	host_[sizeof(host_) - 1] = '\0';
	user_[sizeof(user_) - 1] = '\0';
	pass_[sizeof(pass_) - 1] = '\0';
	database_[sizeof(database_) - 1] = '\0';
	port_ = (port == 0) ? 33060 : port;
}


void DB::connect(void)
{
	mysqlx_session_t *sess;
	mysqlx_error_t *err = nullptr;

	if (sess_) {
		pr_debug("[DB] Closing connection for reconnect...");
		close();
		pr_debug("[DB] Reconnecting to %s:%d...", host_, port_);
	} else {
		pr_debug("[DB] Connecting to %s:%d...", host_, port_);
	}

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


static void DBChkFail(int ret, mysqlx_session_t *sess_)
{
	if (ret != RESULT_OK) {
		char buf[512];
		snprintf(buf, sizeof(buf), "MySQL Error: %s (code: %d)",
			 mysqlx_error_message(sess_), mysqlx_error_num(sess_));
		throw std::runtime_error(std::string(buf));
	}
}


void DB::beginTransaction(void)
{
	DBChkFail(mysqlx_transaction_begin(sess_), sess_);
}


void DB::commit(void)
{
	DBChkFail(mysqlx_transaction_commit(sess_), sess_);
}


void DB::rollback(void)
{
	DBChkFail(mysqlx_transaction_rollback(sess_), sess_);
}


void DB::close(void)
{
	if (sess_) {
		pr_debug("[DB] Closing connection...");
		mysqlx_session_close(sess_);
		sess_ = nullptr;
	}
}


DB::~DB(void)
{
	close();
}


DB *DB::create_conn_from_env(void)
{
	uint16_t port_num;
	const char *host, *port, *user, *pass, *database,
		*err_msg = "unknown error";


	host = getenv("TGVISD_DB_HOST");
	if (!host) {
		err_msg = "Missing TGVISD_DB_HOST";
		goto err;
	}


	port = getenv("TGVISD_DB_PORT");
	if (!port) {
		err_msg = "Missing TGVISD_DB_PORT";
		goto err;
	}
	port_num = (uint16_t)atoi(port);


	user = getenv("TGVISD_DB_USER");
	if (!user) {
		err_msg = "Missing TGVISD_DB_USER";
		goto err;
	}


	pass = getenv("TGVISD_DB_PASS");
	if (!pass) {
		err_msg = "Missing TGVISD_DB_PASS";
		goto err;
	}


	database = getenv("TGVISD_DB_NAME");
	if (!database) {
		err_msg = "Missing TGVISD_DB_NAME";
		goto err;
	}


	return new DB(host, port_num, user, pass, database);

err:
	throw std::runtime_error(std::string(err_msg));
	return nullptr;
}

} /* namespace tgvisd */
