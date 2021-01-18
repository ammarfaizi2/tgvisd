
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <iostream>
#include <unistd.h>
#include <TeaBot/Responses/MyMessage.hpp>

namespace TeaBot::Responses {

/**
 * @param std::shared_ptr<Response> res
 */
MyMessage::MyMessage(std::shared_ptr<Response> res):
    res_(res)
{
}

/**
 * @return void
 */
void MyMessage::run()
{
    auto &update_ = res_->update_;

    std::cout << to_string(update_) << std::endl;
    td_api::object_ptr<td::td_api::message> &msg = update_.message_;

    if (msg->content_->get_id() != td_api::messageText::ID) {
        /* Skip non text message. */
        return;
    }
}

} /* namespace TeaBot::Responses */
