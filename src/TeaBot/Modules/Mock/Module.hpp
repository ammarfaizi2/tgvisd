
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Mock
 */

#ifndef TEABOT__MODULES__MOCK__MODULE_HPP
#define TEABOT__MODULES__MOCK__MODULE_HPP

#include <cstring>
#include <TeaBot/TeaBotModule.hpp>

using TeaBot::TeaBotModule;

namespace TeaBot::Modules::Mock {

class Module : public TeaBotModule
{
private:
    std::string replied_text_;
    bool check_replied_msg();

public:
    inline static bool match(std::shared_ptr<Response> &res)
    {
        const std::string &text  = res->getText();
        const char        *ctext = text.c_str();
        size_t            len    = text.size();

        if (len < 5)
            return false;

        ctext++;
        if (memcmp(ctext, "mock", 4) != 0)
            return false;

        ctext += 4;
        len   -= 5;
        Module mod(res);
        mod.run();

        return true;
    }

    void run();
    inline Module(std::shared_ptr<Response> &res):
        TeaBotModule(res)
    {}
};

} /* namespace TeaBot::Modules::Mock */

#endif
