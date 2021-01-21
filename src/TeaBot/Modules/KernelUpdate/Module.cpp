
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::KernelUpdate
 */

#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/KernelUpdate/Module.hpp>

namespace TeaBot::Modules::KernelUpdate {

/**
 * @return void
 */
void Module::run()
{
    char   *ret;
    size_t len;
    TeaBot::TeaCurl cr("https://www.kernel.org/releases.json");
    cr.execute();

    ret = cr.get_output();
    len = cr.get_outlen();
}

} /* namespace TeaBot::Modules::KernelUpdate */
