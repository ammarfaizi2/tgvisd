
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <TeaBot/Responses/MyMessage/Handlers/ShellExec.hpp>


inline static auto &update_;
inline static auto &msg_;
inline static auto &con_;

/**
 * @param std::shared_ptr<Response> &re
 */
inline MyMessageHandler(std::shared_ptr<Response> &res):
    res_(res),
    update_(res->update_),
    msg_(res->update_.message_),
    con_(msg->content_)
{
}

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
inline void run()
{
    std::string &text  = static_cast<td_api::messageText &>(*con).text_->text_;
    const char  *ctext = text.c_str();
    size_t      len    = text.length();

    if (is_cmd_format(ctext, len)) {
        
    }
}
