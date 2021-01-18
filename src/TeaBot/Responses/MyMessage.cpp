
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <iostream>
#include <unistd.h>
#include <TeaBot/Responses/MyMessage.hpp>

#include <TeaBot/Modules/ShellExec/Module.hpp>

#define TeaModules TeaBot::Modules

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
    td_api::object_ptr<td::td_api::message> &msg = update_.message_;

    if (msg->content_->get_id() == td_api::messageText::ID) {
        handle_text_message();
        return;
    }
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
void MyMessage::handle_text_message()
{
    auto &update_ = res_->update_;
    std::cout << to_string(update_) << std::endl;

    const std::string &text  = res_->getText();
    const char        *ctext = text.c_str();
    size_t            len    = text.size();

    if (is_cmd_format(ctext, len)) {
        if (TeaModules::ShellExec::Module::exec(res_))
            return;
    }
}

} /* namespace TeaBot::Responses */
