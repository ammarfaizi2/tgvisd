// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tdvisds
 *
 * Print functions.
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#if defined(__linux__)
#  include <pthread.h>
static pthread_mutex_t get_time_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t print_lock    = PTHREAD_MUTEX_INITIALIZER;
#else
#  define pthread_mutex_lock(MUTEX)
#  define pthread_mutex_unlock(MUTEX)
#endif

static __always_inline char *get_time(char *buf)
{
	size_t len;
	char *time_chr;
	time_t rawtime;
	struct tm *timeinfo;

	pthread_mutex_lock(&get_time_lock);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	time_chr = asctime(timeinfo);
	len = strlen(time_chr);
	memcpy(buf, time_chr, len);
	buf[len] = '\0';
	pthread_mutex_unlock(&get_time_lock);
	return buf;
}


void __attribute__((format(printf, 1, 2))) __pr_notice(const char *fmt, ...)
{
	va_list vl;
	char buf[32];

	va_start(vl, fmt);
	pthread_mutex_lock(&print_lock);
	printf("[%s] ", get_time(buf));
	vprintf(fmt, vl);
	putchar('\n');
	pthread_mutex_unlock(&print_lock);
	va_end(vl);
}


void __attribute__((format(printf, 1, 2))) __pr_error(const char *fmt, ...)
{
	va_list vl;
	char buf[32];

	va_start(vl, fmt);
	pthread_mutex_lock(&print_lock);
	printf("[%s] Error: ", get_time(buf));
	vprintf(fmt, vl);
	putchar('\n');
	pthread_mutex_unlock(&print_lock);
	va_end(vl);
}


void __attribute__((format(printf, 1, 2)))__pr_emerg(const char *fmt, ...)
{
	va_list vl;
	char buf[32];

	va_start(vl, fmt);
	pthread_mutex_lock(&print_lock);
	printf("[%s] Emergency: ", get_time(buf));
	vprintf(fmt, vl);
	putchar('\n');
	pthread_mutex_unlock(&print_lock);
	va_end(vl);
}


void __attribute__((format(printf, 1, 2))) __pr_debug(const char *fmt, ...)
{
	va_list vl;
	char buf[32];

	va_start(vl, fmt);
	pthread_mutex_lock(&print_lock);
	printf("[%s] Debug: ", get_time(buf));
	vprintf(fmt, vl);
	putchar('\n');
	pthread_mutex_unlock(&print_lock);
	va_end(vl);
}


void __attribute__((format(printf, 3, 4)))
__panic(const char *file, int lineno, const char *fmt, ...)
{
	va_list vl;
	puts("=======================================================");
	printf("Emergency: Panic - Not syncing: ");
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
	putchar('\n');
	printf("Panic at %s:%d\n", file, lineno);
	#define dump_stack()
	/* TODO: Write real dump_stack() */
	dump_stack();
	#undef dump_stack
	puts("==================================================3=====");
	fflush(stdout);
	abort();
}
