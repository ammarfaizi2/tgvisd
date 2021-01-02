
#include "TeaBot.hpp"

namespace TeaBot {


/**
 * @param uint32_t   api_id
 * @param const char *api_hash
 * @param const char *data_path
 *
 * Constructor.
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    handler(api_id, api_hash, data_path)
{
}


/**
 * Destructor.
 */
TeaBot::~TeaBot()
{
}


/**
 * @return void
 */
void TeaBot::run()
{

}


} /* namespace TeaBot */
