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
#include <mysqlx/xapi.h>

#define CRUD_CHECK(C, S)						\
	if (!C) {							\
		char buf[2048];						\
		snprintf(buf, sizeof(buf),				\
			 "\nError! %s", mysqlx_error_message(S));	\
		throw std::string(buf);					\
	}


#define IS_OK(R, C)							\
	if (R != RESULT_OK) {						\
		char buf[2048];						\
		snprintf(buf, sizeof(buf),				\
			 "\nError! %s", mysqlx_error_message(C));	\
		throw std::string(buf);					\
	}


#define RESULT_CHECK(R, C)						\
	if (!R) {							\
		char buf[2048];						\
		snprintf(buf, sizeof(buf),				\
			 "\nError! %s", mysqlx_error_message(C));	\
		throw std::string(buf);					\
	}

#endif /* #ifndef TGVISD__DBCOMMON_HPP */
