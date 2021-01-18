
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__RESPONSE__MYMESSAGE_HPP
#define TEABOT__RESPONSE__MYMESSAGE_HPP

#include <TeaBot/Response.hpp>

using TdTeaBot::TdHandler;

namespace TeaBot::Responses {

class MyMessage
{
private:
    std::shared_ptr<Response> res_;

    void handle_text_message();
public:
    MyMessage(std::shared_ptr<Response> res);
    void run();
};

} /* namespace TeaBot::Responses */

#endif /* #ifndef TEABOT__RESPONSE__MYMESSAGE_HPP */
