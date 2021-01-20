
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Debug
 */

#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/Debug/Module.hpp>

namespace TeaBot::Modules::Debug {

/**
 * @return void
 */
void Module::run()
{
    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto imt       = td_api::make_object<td_api::inputMessageText>();
    auto text      = td_api::make_object<td_api::formattedText>();
    text->text_    = std::move(to_string(update_));
    imt->text_     = std::move(text);

    auto rmsg         = td_api::make_object<td_api::editMessageText>();
    rmsg->chat_id_    = res_->chat_id_;
    rmsg->message_id_ = msg->id_;
    rmsg->input_message_content_ = std::move(imt);
    res_->handler_->send_query(std::move(rmsg), {});
}

} /* namespace TeaBot::Modules::Debug */

