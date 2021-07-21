
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <iostream>
#include <TeaBot/TeaBot.hpp>
#include <TeaBot/Response.hpp>

#if defined(__linux__)
#include <signal.h>
#endif

namespace TeaBot {

#if defined(__linux__)
bool is_signaled	   = false;
bool is_sighandler_set = false;
std::mutex sig_mutex;

/**
 * @param int sig
 * @return void
 */
static void teabot_sighandler(int sig)
{
	is_signaled = true;
	std::cout << "\nGot an interrupt signal!" << std::endl;
	(void)sig;
}
#endif /* #if defined(__linux__) */


/**
 * @param uint32_t api_id
 * @param const char *api_hash
 * @param const char *data_path
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
	handler_(std::make_shared<TdHandler>(api_id, api_hash, data_path))
{

#if defined(__linux__)
	sig_mutex.lock();
	__sync_synchronize();
	if (!is_sighandler_set) {
		signal(SIGINT, teabot_sighandler);
		signal(SIGHUP, teabot_sighandler);
		signal(SIGTERM, teabot_sighandler);
		is_sighandler_set = true;
	}
	sig_mutex.unlock();
#endif /* #if defined(__linux__) */


	handler_->callback.updateNewMessage =
		[this](td_api::updateNewMessage &update) {
			this->updateNewMessage(update);
		};
}

/**
 * Destructor.
 */
TeaBot::~TeaBot()
{
	handler_->close();
}


/**
 * @return void
 */
void TeaBot::run()
{
	const int timeout = 1;

	while (true) {

#if defined(__linux__)
		if (is_signaled)
			break;
#endif

		handler_->loop(timeout);
	}
}


/**
 * @param td_api::updateNewMessage &update
 * @return void
 */
void TeaBot::updateNewMessage(td_api::updateNewMessage &update)
{
	td_api::object_ptr<td::td_api::message> &msg = update.message_;

	if (msg->content_->get_id() != td_api::messageText::ID) {
		/* Skip non text message. */
		return;
	}

	std::shared_ptr<Response> resObj {
		std::make_shared<Response>(std::move(update), handler_)
	};

	resObj->setSelfPtr(resObj);
	std::thread resThread(&Response::run, std::move(resObj));
	resThread.detach();
}

} /* namespace TeaBot */
