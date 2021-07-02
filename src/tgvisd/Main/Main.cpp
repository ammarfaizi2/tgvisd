// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#include <cstdio>
#include <cerrno>
#include <thread>
#include <cstring>
#include <iostream>
#include <signal.h>

#if defined(__linux__)
#include <unistd.h>
#endif


#include "Main.hpp"


namespace tgvisd::Main {


#if defined(__linux__)

static std::mutex sig_mutex;
static bool is_signaled = false;
static bool is_sighandler_set = false;


static void main_sighandler(int sig)
{
	if (!is_signaled) {
		printf("\nGot an interrupt signal %d\n", sig);
		printf("Syncing...\n");
		is_signaled = true;
		sync();
	}
}
#endif


Main::Main(uint32_t api_id, const char *api_hash, const char *data_path):
	td_(std::make_shared<tgvisd::Td::Td>(api_id, api_hash, data_path))
{


#if defined(__linux__)
	/*
	 * Use signal interrupt handler to kill the
	 * process gracefully on Linux.
	 *
	 * Mutex note:
	 * We may have several instances calling
	 * Main::Main simultaneously, so we need a
	 * mutex here.
	 *
	 * Signal handler is only required to be
	 * set once.
	 */
	sig_mutex.lock();
	if (__builtin_expect(!is_sighandler_set, 0)) {
		signal(SIGINT, main_sighandler);
		signal(SIGHUP, main_sighandler);
		signal(SIGTERM, main_sighandler);
		is_sighandler_set = true;
	}
	sig_mutex.unlock();
#endif

	td_->callback.updateNewMessage = [this](td_api::updateNewMessage &update) {
		this->updateNewMessage(update);
	};
}


Main::~Main(void)
{
	td_->close();
}


void Main::run(void)
{
	constexpr int timeout = 1;

	while (true) {


#if defined(__linux__)
		/*
		 * We have a signal handler on Linux.
		 *
		 * This can ensure the destructor is called
		 * when we get interrupted. For example, if
		 * the user presses CTRL + C (got SIGINT)
		 */
		if (__builtin_expect(is_signaled, 0))
			break;
#endif	

		td_->loop(timeout);
	}
}


void Main::updateNewMessage(td_api::updateNewMessage &update)
{
	td_api::object_ptr<td::td_api::message> &msg = update.message_;

	if (msg->content_->get_id() != td_api::messageText::ID) {
		/* Skip non text message. */
		return;
	}

	auto &message = update.message_;
	auto &content = *(message->content_);

	switch (message->content_->get_id()) {
	case td_api::messageText::ID: {
		auto &text = static_cast<td_api::messageText &>(content).text_;
		std::cout << text->text_ << std::endl;
		break;
	}
	}
}


} /* namespace tgvisd::Main */

#define ACC_PATH_LEN (1024u)


struct tg_account {
	uint32_t	api_id;
	char		api_hash[64];
	char		data_path[ACC_PATH_LEN + sizeof("/storage")];
};


static int load_tg_account(const char *acc_path, struct tg_account *acc)
{
	size_t l;
	FILE *handle;
	char pbuf[ACC_PATH_LEN + 30];


	/*
	 * Read api_id.txt
	 */
	snprintf(pbuf, sizeof(pbuf), "%s/api_id.txt", acc_path);
	handle = fopen(pbuf, "rb");
	if (!handle) {
		int err = errno;
		printf("Error: fopen(\"%s\", \"rb\"): %s\n", pbuf, strerror(err));
		return err;
	}
	/*
	 * Borrow the api_hash buffer for temporary
	 * read (before strtoul).
	 */
	if (!fgets(acc->api_hash, sizeof(acc->api_hash), handle)) {
		printf("Cannot read api_hash from \"%s\"\n", pbuf);
		return EINVAL;
	}
	acc->api_id = strtoul(acc->api_hash, NULL, 10);
	fclose(handle);


	/*
	 * Read api_hash.txt
	 */
	printf("test\n");
	snprintf(pbuf, sizeof(pbuf), "%s/api_hash.txt", acc_path);
	handle = fopen(pbuf, "rb");
	if (!handle) {
		int err = errno;
		printf("Error: fopen(\"%s\", \"rb\"): %s\n", pbuf, strerror(err));
		return err;
	}
	if (!fgets(acc->api_hash, sizeof(acc->api_hash), handle)) {
		printf("Cannot read api_hash from \"%s\"\n", pbuf);
		return EINVAL;
	}
	fclose(handle);

	/*
	 * Cut the newline.
	 */
	l = strlen(acc->api_hash);
	if ((l > 0) && (acc->api_hash[l - 1] == '\n'))
		acc->api_hash[l - 1] = '\0';

	snprintf(acc->data_path, sizeof(acc->data_path), "%s/storage", acc_path);
	return 0;
}


static int run_account(struct tg_account *acc)
{
	tgvisd::Main::Main main(acc->api_id, acc->api_hash, acc->data_path);
	printf("Starting daemon for data_path \"%s\"...\n", acc->data_path);
	main.run();
	return 0;
}


static int run_accounts(std::vector<struct tg_account> *accounts)
{
	int ret = 0;
	size_t len = accounts->size();

	if (len == 0)
		return EINVAL;

	for (size_t i = 1; i < len; i++) {
		/*
		 * TODO: Run (*accounts)[1] to (*accounts[len - 1]) on
		 *       independent threads.
		 */
	}


	/* Run accounts[0] on the main thread. */
	run_account(&(*accounts)[0]);

	return ret;
}


int main(int argc, char *argv[])
{
	int ret = 0;
	FILE *handle;
	char *p, pbuf[ACC_PATH_LEN];
	std::vector<struct tg_account> accounts;

	if (argc != 2) {
		printf("Usage: %s <account_list_file>\n", argv[0]);
		return 1;
	}

	handle = fopen(argv[1], "rb");
	if (!handle) {
		int err = errno;
		printf("Error: fopen(\"%s\", \"rb\"): %s\n", argv[1], strerror(err));
		return err;
	}

	while ((p = fgets(pbuf, sizeof(pbuf), handle))) {
		size_t l;
		struct tg_account acc;

		l = strlen(p);
		if (l < 2)
			continue;

		/*
		 * Cut the newline.
		 */
		if (p[l - 1] == '\n')
			p[--l] = '\0';

		ret = load_tg_account(p, &acc);
		if (ret) {
			printf("Skipping \"%s\"...\n", p);
			continue;
		}

		accounts.push_back(acc);
	}
	fclose(handle);
	ret = run_accounts(&accounts);
	return ret;
}
