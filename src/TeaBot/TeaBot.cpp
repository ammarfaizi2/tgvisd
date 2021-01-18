
#include <TeaBot/TeaBot.hpp>

namespace TeaBot {

TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    handler_(std::make_shared<TdHandler>(api_id, api_hash, data_path))
{
}

TeaBot::~TeaBot()
{
}

} /* namespace TeaBot */
