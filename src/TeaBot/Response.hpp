
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__RESPONSE_HPP
#define TEABOT__RESPONSE_HPP

#include <memory>
#include <TdTeaBot/TdHandler.hpp>

using TdTeaBot::TdHandler;

namespace TeaBot {

class Response
{
private:

public:
    td_api::updateNewMessage   update_;
    std::shared_ptr<TdHandler> handler_;
    std::shared_ptr<Response>  self_;

    int64_t chat_id_;
    int64_t sender_id_;
    std::string sender_name_;

    Response(td_api::updateNewMessage update,
             std::shared_ptr<TdHandler> handler);

    ~Response();

    void run();

    inline void setSelfPtr(std::shared_ptr<Response> self)
    {
        self_ = self;
    }
};

} /* namespace TeaBot */

#endif /* #ifndef TEABOT__RESPONSE_HPP */
