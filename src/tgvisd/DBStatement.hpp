// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd
 *
 * Copyright (C) 2021  Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__DBSTATEMENT_HPP
#define TGVISD__DBSTATEMENT_HPP

#include <mysqlx/xapi.h>
#include <tgvisd/common.hpp>

namespace tgvisd {

class DBStatement {
public:
	DBStatement(void);
	~DBStatement(void);
	DBStatement(DB *db);
private:
	DB *db_ = nullptr;
};

} /* namespace tgvisd */

#endif /* #ifndef TGVISD__DBSTATEMENT_HPP */
