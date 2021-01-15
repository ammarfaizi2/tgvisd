
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#ifndef __TeaBot__Responses_HPP
#define __TeaBot__Responses_HPP

#include "TdLibHandler.hpp"

namespace TeaBot {

class Responses
{
private:
    int64_t chat_id_;
    int64_t sender_id_;
    std::string sender_name_;

    TdLibHandler *handler_;
    td_api::updateNewMessage update_;

public:
    Responses(td_api::updateNewMessage update, TdLibHandler *handler);
    void run();
    void handle_message();
    void handle_self_message();
};

} /* namespace TeaBot */

#endif /* #ifndef __TeaBot__Responses_HPP */

