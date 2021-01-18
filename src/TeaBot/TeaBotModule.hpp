
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__TEABOTMODULE_HPP
#define TEABOT__TEABOTMODULE_HPP

#include <TeaBot/Response.hpp>

namespace TeaBot {

class TeaBotModule
{
protected:
    std::shared_ptr<Response> &res_;

public:
    inline TeaBotModule(std::shared_ptr<Response> &res):
        res_(res) {};

    inline static bool exec(std::shared_ptr<Response> &res)
    {
        return false;
    }
};

} /* namespace TeaBot */

#endif
