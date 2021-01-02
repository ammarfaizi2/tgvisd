
#include "TeaBot.hpp"

namespace TeaBot {

/**
 * @param uint32_t   api_id
 * @param const cahr *api_hash
 * @param const char *storage_path
 *
 * Constructor.
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *storage_path):
    handler(api_id, api_hash, storage_path)
{
  handler.setUpdateNewMessageCallback(
    [this](auto a, auto b){
      this->onUpdateNewMessage(a, b);
    }
  );
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


/**
 * @param td_api::updateNewMessage &update
 * @param TdLibHandler             *handler
 * @return void
 */
void
TeaBot::onUpdateNewMessage(
  td_api::updateNewMessage &update,
  TdLibHandler *handler
)
{
  std::cout << to_string(update) << std::endl;

  td::tl_object_ptr<td_api::message> &msg = update.message_;



  int64_t id            = msg->id_;
  int64_t chat_id       = msg->chat_id_;
  bool    can_be_edited = msg->can_be_edited_;

  if (can_be_edited) {
    this->handleSelfMessage(msg);
  }

  // int64_t chat_id   = 
  // int64_t sender_id = update.message_->sender_->user_id_;
  // std::string text  = "Hello!";

  // auto send_message    = td_api::make_object<td_api::sendMessage>();
  // auto message_content = td_api::make_object<td_api::inputMessageText>();

  // send_message->chat_id_ = chat_id;
  // message_content->text_ = td_api::make_object<td_api::formattedText>();
  // message_content->text_->text_ = std::move(text);
  // send_message->input_message_content_ = std::move(message_content);

  // handler->send_query(std::move(send_message), {});
}


} /* namespace TeaBot */
