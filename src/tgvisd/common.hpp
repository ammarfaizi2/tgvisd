// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__COMMON_HPP
#define TGVISD__COMMON_HPP

#include <cstdio>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdbool>


/*
 * These macros currently just for self-documented code!
 */
#define __must_hold(MUTEX)
#define __releases(MUTEX)
#define __acquires(MUTEX)

#define likely(EXPR)	__builtin_expect((bool)(EXPR), 1)
#define unlikely(EXPR)	__builtin_expect((bool)(EXPR), 0)

#include <tgvisd/print.h>

#endif /* #ifndef TGVISD__COMMON_HPP */
