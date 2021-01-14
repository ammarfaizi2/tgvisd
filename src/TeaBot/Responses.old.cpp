
#include "Responses.hpp"

#include <cstdio>
#include <errno.h>
#include <cstring>

namespace TeaBot {


/**
 * @param td_api::updateNewMessage update
 * @param TdLibHandler             *handler
 *
 * Constructor.
 */
Responses::Responses(td_api::updateNewMessage update, TdLibHandler *handler):
        update_(std::move(update))
    ,   handler_(handler)
{
}


/**
 * @return void
 */
void Responses::run()
{
    chat_id_ = update_.message_->chat_id_;
    td_api::downcast_call(
        *(update_.message_->sender_),
        overloaded(
            [this](td_api::messageSenderUser &user) {
                sender_id_   = user.user_id_;
                sender_name_ = handler_->get_user_name(sender_id_);
            },
            [this](td_api::messageSenderChat &chat) {
                sender_id_   = chat.chat_id_;
                sender_name_ = handler_->get_chat_title(sender_id_);
            }
        )
    );

    if (handler_->get_user_id() == sender_id_) {
        handle_self_message();
        return;
    }
}


#include "Responses/handle_shell_exec.hpp"
#include "Responses/handle_debug_msg.hpp"
#include "Responses/handle_mock_string.hpp"


/**
 * @return void
 */
void Responses::handle_message()
{
    td_api::object_ptr<td::td_api::message> &msg = update_.message_;

    if (msg->content_->get_id() != td_api::messageText::ID) {
        /* Skip non text message. */
        return;
    }


    std::string &text  = static_cast<td_api::messageText &>(*(msg->content_)).text_->text_;
    const char  *ctext = text.c_str();
    size_t      len    = text.length();
}


/**
 * @param const char *ctext
 * @param size_t     len
 * @return bool
 */
static bool is_cmd_format(const char *ctext, size_t len)
{
    if (len < 2)
        return false;

    char c = ctext[0];
    if (!(c == '!' || c == '/' || c == '.'))
        return false;

    return true;
}


/**
 * @return void
 */
void Responses::handle_self_message()
{
    td_api::object_ptr<td::td_api::message> &msg = update_.message_;

    if (msg->content_->get_id() != td_api::messageText::ID) {
        /* Skip non text message. */
        return;
    }


    std::string &text  = static_cast<td_api::messageText &>(*(msg->content_)).text_->text_;
    const char  *ctext = text.c_str();
    size_t      len    = text.length();


    if (!is_cmd_format(ctext, len))
        return;

    if (handle_shell_exec(ctext, len, handler_, msg, chat_id_))
        return;

    if (handle_debug_msg(ctext, len, handler_, msg, chat_id_))
        return;

    if (handle_debug_msg(ctext, len, handler_, msg, chat_id_))
        return;
}

} /* namespace TeaBot */
