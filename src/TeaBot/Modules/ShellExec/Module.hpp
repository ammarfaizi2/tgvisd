
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::ShellExec
 */

#ifndef TEABOT__MODULES__SHELLEXEC__MODULE_HPP
#define TEABOT__MODULES__SHELLEXEC__MODULE_HPP

#include <cstring>
#include <TeaBot/TeaBotModule.hpp>

using TeaBot::TeaBotModule;

namespace TeaBot::Modules::ShellExec {

class Module : public TeaBotModule
{
public:
    inline static bool exec(std::shared_ptr<Response> &res)
    {
        const std::string &text  = res->getText();
        const char        *ctext = text.c_str();
        size_t            len    = text.size();

        if (len < 5)
            return false;

        ctext++;
        if (memcmp(ctext, "sh ", 3) != 0)
            return false;

        ctext += 3;
        len   -= 4;
        Module mod(res);
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
