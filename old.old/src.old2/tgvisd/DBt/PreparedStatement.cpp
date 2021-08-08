// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::DBt
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include "PreparedStatement.hpp"

namespace tgvisd::DBt {

PreparedStatement::~PreparedStatement(void)
{
	if (res_) {
		mysqlx_free(res_);
		res_ = nullptr;
	}
}

} /* namespace tgvisd::DBt */
