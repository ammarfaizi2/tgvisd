
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/Sed/Module.hpp>

namespace TeaBot::Modules::Sed {

#define SHELL_WRAP_STR "exec /usr/bin/bash -c "

/**
 * @param const char *cmd
 * @param size_t     len
 * @return void
 */
void Module::run(const char *cmd, size_t len)
{
}

} /* namespace TeaBot::Modules::Sed */
