
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::ShellExec
 */

#ifndef TEABOT__MODULES__SHELLEXEC__MODULE_HPP
#define TEABOT__MODULES__SHELLEXEC__MODULE_HPP

#include <map>
#include <cstring>
#include <TeaBot/TeaBotModule.hpp>

using TeaBot::TeaBotModule;

namespace TeaBot::Modules::ShellExec {

static const std::map<int64_t, const char *> sudoers = {
    {243692601, "Ammar Faizi"}
};

class Module : public TeaBotModule
{
private:
    bool edit_msg_;

public:
    inline static bool match(std::shared_ptr<Response> &res)
    {
        bool              edit_msg_ = true;
        const std::string &text     = res->getText();
        const char        *ctext    = text.c_str();
        size_t            len       = text.size();

        /* Skip message with length less than 5. */
        if (len < 5)
            return false;

        ctext++;

        if (memcmp(ctext, "sh ", 3) == 0) {
            ctext += 3;
            len   -= 4;
            edit_msg_ = false;
        } else
        if (memcmp(ctext, "shr ", 4) == 0) {
            ctext += 4;
            len   -= 5;
        } else {
            /* Message doesn't match. */
            return false;
        }


        if (!res->is_self_msg_) {

            /* If this is not my message, then check
               whether the sender is sudoer or not. */
            auto it = sudoers.find(res->sender_id_);
            if (it == sudoers.end()) {
                /* Skip non sudoer user. */
                return false;
            }
            edit_msg_ = false;
        }


        Module mod(res);
        mod.edit_msg_ = edit_msg_;
        mod.run(ctext, len);

        return true;
    }

    void run(const char *cmd, size_t len);
    inline Module(std::shared_ptr<Response> &res):
        TeaBotModule(res)
    {}
};

} /* namespace TeaBot::Modules::ShellExec */

#endif
