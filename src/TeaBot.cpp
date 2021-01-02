
#include "TeaBot.hpp"

namespace TeaBot {

/**
 * @param const char *storage_path
 *
 * Constructor.
 */
TeaBot::TeaBot(const char *storage_path):
    storage_path_(storage_path)
  , handler(storage_path)
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
void
TeaBot::run()
{
  handler.loop();
}

} /* namespace TeaBot */
