
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__TEABOT_HPP
#define TEABOT__TEABOT_HPP

#include <TdTeaBot/TdHandler.hpp>

using TdTeaBot::TdHandler;

namespace TeaBot {

class TeaBot
{
private:
	std::shared_ptr<TdHandler> handler_;

	void updateNewMessage(td_api::updateNewMessage &update);

public:
	TeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
	~TeaBot();
	void run();
};

} /* namespace TeaBot */

#endif /* #ifndef TEABOT__TEABOT_HPP */
