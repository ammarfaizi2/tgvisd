
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */


#include <TeaBot/TeaBot.hpp>
#include <TeaBot/Response.hpp>

namespace TeaBot {

/**
 * @param uint32_t api_id
 * @param const char *api_hash
 * @param const char *data_path
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    handler_(std::make_shared<TdHandler>(api_id, api_hash, data_path))
{
    handler_->callback.updateNewMessage =
        [this](td_api::updateNewMessage &update) {
            this->updateNewMessage(update);
        };
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


/**
 * @param td_api::updateNewMessage &update
 * @return void
 */
void TeaBot::updateNewMessage(td_api::updateNewMessage &update)
{
    std::shared_ptr<Response> resObj {
        std::make_shared<Response>(std::move(update), handler_)
    };
    resObj->setSelfPtr(resObj);

    std::thread resThread(&Response::run, resObj);
    resThread.detach();
}

} /* namespace TeaBot */
