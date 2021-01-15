
#include "self_message/mock_msg.hpp"
#include "self_message/debug_msg.hpp"
#include "self_message/shell_exec.hpp"

/**
 * @param const char *ctext
 * @param size_t     len
 * @return bool
 */
inline static bool is_cmd_format(const char *ctext, size_t len)
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


    if (is_cmd_format(ctext, len)) {

        if (handle_shell_exec(ctext, len, handler_, msg, chat_id_))
            return;

        if (handle_debug_msg(ctext, len, handler_, msg, chat_id_))
            return;

        if (handle_mock_msg(ctext, len, handler_, msg, chat_id_))
            return;
    }
}
