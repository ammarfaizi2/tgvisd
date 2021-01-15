
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#include "TdLibHandler.hpp"

namespace TeaBot {

using Object  = td_api::object_ptr<td_api::Object>;

/**
 * @param uint32_t    api_id
 * @param const char  *api_hash
 * @param const char  *data_path
 *
 * Constructor.
 */
TdLibHandler::TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path):
    api_id_(api_id)
  , api_hash_(api_hash)
  , data_path_(data_path)
{
    td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(1));
    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_      = client_manager_->create_client_id();
    send_query(td_api::make_object<td_api::getOption>("version"), {});
}


/**
 * @return void
 */
void TdLibHandler::loop()
{
    if (need_restart_) {
        restart();
        return;
    }

    if (!are_authorized_) {
        process_response(client_manager_->receive(10));
        return;
    }

    handle_event_loop();
}


/**
 * @return void
 */
void TdLibHandler::restart()
{
    client_manager_.reset();
    *this = TdLibHandler(api_id_, api_hash_, data_path_);
}


/**
 * @param td_api::object_ptr<td_api::Function>  f
 * @param std::function<void(Object)>           handler
 * @return void
 */
void TdLibHandler::send_query(td_api::object_ptr<td_api::Function> f, 
                              std::function<void(Object)> handler)
{
    auto query_id = next_query_id();
    if (handler)
        handlers_.emplace(query_id, std::move(handler));

    client_manager_->send(client_id_, query_id, std::move(f));
}


/**
 * @return uint64_t
 */
uint64_t TdLibHandler::next_query_id()
{
    return ++current_query_id_;
}


/**
 * @return bool
 */
bool TdLibHandler::handle_event_loop()
{
    while (true) {
        auto response = client_manager_->receive(2);

        if (response.object)
            process_response(std::move(response));
        else
            break;
    }

    /* return false to stop the event loop. */

    return true;
}


/**
 * @param td::ClientManager::Response response
 * @return void
 */
void TdLibHandler::process_response(td::ClientManager::Response response)
{
    if (!response.object)
        return;

    /*std::cout << response.request_id << " " << to_string(response.object) << std::endl;*/

    if (response.request_id == 0) {
        process_update(std::move(response.object));
        return;
    }

    auto it = handlers_.find(response.request_id);
    if (it != handlers_.end())
        it->second(std::move(response.object));
}


/**
 * @param td_api::object_ptr<td_api::Object> update
 */
void TdLibHandler::process_update(td_api::object_ptr<td_api::Object> update)
{
    td_api::downcast_call(
        *update,
        overloaded(
            [this](td_api::updateAuthorizationState &update) {
                authorization_state_ = std::move(update.authorization_state_);
                on_authorization_state_update();
            },
            [this](td_api::updateNewChat &update) {
                chat_title_[update.chat_->id_] = update.chat_->title_;
            },
            [this](td_api::updateChatTitle &update) {
                chat_title_[update.chat_id_] = update.title_;
            },
            [this](td_api::updateUser &update) {
                auto user_id = update.user_->id_;
                users_[user_id] = std::move(update.user_);
            },
            [this](td_api::updateNewMessage &update) {

                if (onUpdateNewMessageCallback) {
                    onUpdateNewMessageCallback(update);
                }

                // auto chat_id = update.message_->chat_id_;
                // std::string sender_name;
                // td_api::downcast_call(
                //     *update.message_->sender_,
                //     overloaded(
                //         [this, &sender_name](td_api::messageSenderUser &user) {
                //             sender_name = get_user_name(user.user_id_);
                //         },
                //         [this, &sender_name](td_api::messageSenderChat &chat) {
                //             sender_name = get_chat_title(chat.chat_id_);
                //         }
                //     )
                // );

                // std::string text;
                // if (update.message_->content_->get_id() == td_api::messageText::ID) {
                //     text = static_cast<td_api::messageText &>(*update.message_->content_).text_->text_;
                // }

                // std::cout
                //     << "Got message: [chat_id:"
                //     << chat_id
                //     << "] [from:"
                //     << sender_name
                //     << "] ["
                //     << text
                //     << "]"
                //     << std::endl;
            },
            [](auto &update) {
            }
        )
    );
}


/**
 * @return void
 */
void TdLibHandler::on_authorization_state_update()
{
    authentication_query_id_++;
    td_api::downcast_call(
        *authorization_state_,
        overloaded(
            [this](td_api::authorizationStateReady &) {
                are_authorized_ = true;
                std::cout << "Got authorization" << std::endl;

                /* Get account user_id. */
                send_query(
                    td_api::make_object<td_api::getMe>(),
                    [this](Object user_obj){
                        if (user_obj->get_id() == td_api::user::ID)
                            user_id_ = (td::move_tl_object_as<td_api::user>(user_obj))->id_;
                    }
                );
            },
            [this](td_api::authorizationStateLoggingOut &) {
                are_authorized_ = false;
                std::cout << "Logging out" << std::endl;
            },
            [this](td_api::authorizationStateClosing &) {
                std::cout << "Closing" << std::endl;
            },
            [this](td_api::authorizationStateClosed &) {
                are_authorized_ = false;
                need_restart_ = true;
                closed_ = true;
                std::cout << "Terminated" << std::endl;
            },
            [this](td_api::authorizationStateWaitCode &) {
                std::cout << "Enter authentication code: " << std::flush;
                std::string code;
                std::cin >> code;
                send_query(td_api::make_object<td_api::checkAuthenticationCode>(code),
                           create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitRegistration &) {
                std::string first_name;
                std::string last_name;
                std::cout << "Enter your first name: " << std::flush;
                std::cin >> first_name;
                std::cout << "Enter your last name: " << std::flush;
                std::cin >> last_name;
                send_query(td_api::make_object<td_api::registerUser>(first_name, last_name),
                           create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitPassword &) {
                std::cout << "Enter authentication password: " << std::flush;
                std::string password;
                std::getline(std::cin, password);
                send_query(td_api::make_object<td_api::checkAuthenticationPassword>(password),
                           create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                std::cout << "Confirm this login link on another device: " << state.link_ << std::endl;
            },
            [this](td_api::authorizationStateWaitPhoneNumber &) {
                std::cout << "Enter phone number: " << std::flush;
                std::string phone_number;
                std::cin >> phone_number;
                send_query(td_api::make_object<td_api::setAuthenticationPhoneNumber>(phone_number, nullptr),
                           create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitEncryptionKey &) {
                std::cout << "Enter encryption key or DESTROY: " << std::flush;
                std::string key;
                std::getline(std::cin, key);
                if (key == "DESTROY")
                    send_query(td_api::make_object<td_api::destroy>(),
                               create_authentication_query_handler());
                else
                    send_query(td_api::make_object<td_api::checkDatabaseEncryptionKey>(std::move(key)),
                               create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitTdlibParameters &) {
                auto params = td_api::make_object<td_api::tdlibParameters>();
                params->use_message_database_ = true;
                params->use_secret_chats_ = true;
                params->api_id_ = this->api_id_;
                params->api_hash_ = this->api_hash_;
                params->database_directory_ = this->data_path_;
                params->system_language_code_ = "en";
                params->device_model_ = "Desktop";
                params->application_version_ = "1.0";
                params->enable_storage_optimizer_ = true;
                send_query(td_api::make_object<td_api::setTdlibParameters>(std::move(params)),
                           create_authentication_query_handler());
            }
        )
    );
}


/**
 * @param int32_t user_id
 * @return std::string
 */
std::string TdLibHandler::get_user_name(int32_t user_id) const
{
    auto it = users_.find(user_id);
    if (it == users_.end())
        return "unknown user";

    return it->second->first_name_ + " " + it->second->last_name_;
}


/**
 * @param int64_t chat_id
 * @return std::string
 */
std::string TdLibHandler::get_chat_title(int64_t chat_id) const
{
    auto it = chat_title_.find(chat_id);

    if (it == chat_title_.end())
        return "unknown chat";

    return it->second;
}


/**
 * @return std::function<void(Object object)>
 */
std::function<void(Object object)> TdLibHandler::create_authentication_query_handler()
{
    return [this, id = authentication_query_id_](Object object) {
        if (id == authentication_query_id_) {
            check_authentication_error(std::move(object));
        }
    };
}


/**
 * @param Object object
 */
void TdLibHandler::check_authentication_error(Object object)
{
    if (object->get_id() == td_api::error::ID) {
        auto error = td::move_tl_object_as<td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;
        on_authorization_state_update();
    }
}


/**
 * @param std::function<void(td_api::updateNewMessage &update, TdLibHandler *handler)>
    onUpdateNewMessageCallback
 * @return void
 */
void TdLibHandler::setCallback(
    std::function<void(td_api::updateNewMessage &update)>
    onUpdateNewMessageCallback
)
{
    this->onUpdateNewMessageCallback = onUpdateNewMessageCallback;
}


/**
 * @return int32_t
 */
int32_t TdLibHandler::get_client_id()
{
    return client_id_;
}

/**
 * @return int32_t
 */
int32_t TdLibHandler::get_user_id()
{
    return user_id_;
}


/**
 * @return void
 */
void TdLibHandler::close()
{
    send_query(td_api::make_object<td_api::close>(), {});
    std::cout << "Waiting for authorizationStateClosed..." << std::endl;
    handle_event_loop();
}

} /* namespace TeaBot */
