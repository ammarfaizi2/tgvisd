
#include "TeaBot.hpp"

namespace TeaBot {

static void handleSelfMessage(td_api::updateNewMessage &update, TdLibHandler *handler);

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
    // 1472415329
    // std::cout << to_string(update) << std::endl;

    std::string sender_name;

    int64_t sender_id  = 0;
    int64_t chat_id    = update.message_->chat_id_;
    int64_t my_user_id = handler->get_user_id();

    td_api::downcast_call(
        *update.message_->sender_,
        overloaded(
            [&](td_api::messageSenderUser &user) {
                sender_id   = user.user_id_;
                sender_name = handler->get_user_name(sender_id);
            },
            [&](td_api::messageSenderChat &chat) {
                sender_id   = chat.chat_id_;
                sender_name = handler->get_chat_title(sender_id);
            }
        )
    );

    if (my_user_id == sender_id) {
        handleSelfMessage(update, handler);
    }
}


/**
 * @return void
 */
static void handleSelfMessage(td_api::updateNewMessage &update, TdLibHandler *handler)
{
    auto &msg = update.message_;
    int64_t chat_id = msg->chat_id_;

    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_ = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = "Edited!";
    emsg->chat_id_    = chat_id;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    handler->send_query(std::move(emsg), {});
}


} /* namespace TeaBot */
