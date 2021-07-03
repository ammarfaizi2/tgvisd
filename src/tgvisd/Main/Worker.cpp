// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstdio>

#include "Main.hpp"
#include "Worker.hpp"

namespace tgvisd::Main {


Worker::Worker(void)
{
}


Worker::Worker(Main *main, uint32_t idx):
	main_(main),
	idx_(idx)
{
}


Worker::~Worker()
{
	stop_ = true;
	if (!thread_)
		return;

	printf("Waiting for thread %u...\n", idx_);
	thread_->join();
	printf("Thread %u has been joined\n", idx_);
	delete thread_;
}


void Worker::spawn(void)
{
	if (thread_)
		return;

	printf("Spawning thread %u...\n", idx_);
	thread_ = new std::thread([this](void) { this->internalWorker(); });
}


void Worker::internalWorker(void)
{
	printf("Thread %u is ready!\n", idx_);
	while (__builtin_expect(!stop_, 1)) {
		sleep(3);
	}
	printf("Thread %u is exiting...\n", idx_);
}


} /* namespace tgvisd::Main */
