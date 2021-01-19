
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Sed
 */

#ifndef TEABOT__MODULES__SED__MODULE_HPP
#define TEABOT__MODULES__SED__MODULE_HPP

#include <cstring>
#include <TeaBot/TeaBotModule.hpp>

using TeaBot::TeaBotModule;

namespace TeaBot::Modules::Sed {

class Module : public TeaBotModule
{
public:
    inline static bool match(std::shared_ptr<Response> &res)
    {
        const std::string &text  = res->getText();
        const char        *ctext = text.c_str();
        size_t            len    = text.size();

        if (len < 5)
            return false;

        ctext++;
        if (memcmp(ctext, "sed ", 4) != 0)
            return false;

        ctext += 3;
        len   -= 5;
        Module mod(res);
        mod.run(ctext, len);

        return true;
    }

    void run(const char *cmd, size_t len);
    inline Module(std::shared_ptr<Response> &res):
        TeaBotModule(res)
    {}
};

} /* namespace TeaBot::Modules::Sed */

#endif
