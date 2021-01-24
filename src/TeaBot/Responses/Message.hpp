
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__RESPONSE__MESSAGE_HPP
#define TEABOT__RESPONSE__MESSAGE_HPP

#include <TeaBot/Response.hpp>

using TdTeaBot::TdHandler;

namespace TeaBot::Responses {

class Message
{
private:
	std::shared_ptr<Response> res_;

	void handle_text_message();
public:
	Message(std::shared_ptr<Response> res);
	void run();
};

} /* namespace TeaBot::Responses */

#endif /* #ifndef TEABOT__RESPONSE__MESSAGE_HPP */
