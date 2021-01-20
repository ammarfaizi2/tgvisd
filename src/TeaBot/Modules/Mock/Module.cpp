
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Sed
 */

#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/Sed/Module.hpp>

namespace TeaBot::Modules::Sed {

/**
 * @return bool
 */
bool Module::check_replied_msg()
{
    std::mutex  mut;
    bool        action = false;

    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto getMsg    = td_api::make_object<td_api::getMessage>(
        msg->reply_in_chat_id_,
        msg->reply_to_message_id_
    );

    std::string &replied_text_ = this->replied_text_;

    mut.lock();
    res_->handler_->send_query(
        std::move(getMsg),
        [&action, &replied_text_, &mut] (auto object) {

            if (object->get_id() == td_api::error::ID)
                goto ret;

            {
                auto msg = td::move_tl_object_as<td_api::message>(object);
                if (msg->content_->get_id() != td_api::messageText::ID)
                    goto ret; /* Skip non text message. */

                action         = true;
                auto &c        = msg->content_;
                auto &rtext    = static_cast<td_api::messageText &>(*c).text_;
                replied_text_  = std::move(rtext->text_);
            }

        ret:
            mut.unlock();
        }
    );
    mut.lock();

    return action;
}

/**
 * @param const char *pat
 * @param size_t     len
 * @return void
 */
#define PRINT_CMD "printf \"%s\" "
#define SED_CMD   " | sed -r "
void Module::run(const char *pat, size_t len)
{
    if (!check_replied_msg())
        return;

    size_t alloc = 0; /* Heap allocation size. */
    size_t esc_len = 0;

    const char *crep      = replied_text_.c_str();
    size_t      crep_len  = replied_text_.size();

}

} /* namespace TeaBot::Modules::Sed */

