
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
    handler_(api_id, api_hash, data_path)
{
  handler_.setCallback([this](td_api::updateNewMessage &update, TdLibHandler *handler){
    this->onUpdateNewMessage(update, handler);
  });
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
  handler_.loop();
}


/**
 * @param td_api::updateNewMessage &update
 * @param TdLibHandler             *handler
 * @return void
 */
void TeaBot::onUpdateNewMessage(td_api::updateNewMessage &update, TdLibHandler *handler)
{

}

} /* namespace TeaBot */
