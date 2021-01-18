
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TdTeaBot
 */

#include <TeaBot/TeaBot.hpp>

namespace TeaBot {

/**
 * @param uint32_t api_id
 * @param const char *api_hash
 * @param const char *data_path
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    handler_(std::make_shared<TdHandler>(api_id, api_hash, data_path))
{
}

/**
 * Destructor.
 */
TeaBot::~TeaBot()
{
}

} /* namespace TeaBot */
