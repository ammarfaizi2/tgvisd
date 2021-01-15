
#include <TDTeaBot/TDTeaBot.hpp>

namespace TDTeaBot {

/**
 * @param uint32_t api_id
 * @param const char *api_hash
 * @param const char *data_path
 */
TDTeaBot::TDTeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    api_id_(api_id),
    api_hash_(api_hash),
    data_path_(data_path)
{
}

/**
 * Destructor.
 */
TDTeaBot::~TDTeaBot()
{
}

} /* namespace TDTeaBot */
