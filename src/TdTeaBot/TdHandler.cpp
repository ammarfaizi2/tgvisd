
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TdTeaBot
 */

#include <cstdio>
#include <iostream>
#include <TdTeaBot/TdHandler.hpp>

namespace TdTeaBot {


using Object = td_api::object_ptr<td_api::Object>;


/**
 * @param uint32_t api_id
 * @param const char *api_hash
 * @param const char *data_path
 */
TdHandler::TdHandler(uint32_t api_id, const char *api_hash,
                     const char *data_path):
    api_id_(api_id),
    api_hash_(api_hash),
    data_path_(data_path)
{
    td::ClientManager::execute(
        td_api::make_object<td_api::setLogVerbosityLevel>(1));

    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_      = client_manager_->create_client_id();

    send_query(td_api::make_object<td_api::getOption>("version"), {});
}


/**
 * @return void
 */
void TdHandler::restart()
{
    client_manager_.reset();

    closed_                  = false;
    need_restart_            = false;
    are_authorized_          = false;
    current_query_id_        = 0;
    authentication_query_id_ = 0;
}


/**
 * @param td_api::object_ptr<td_api::Function>  f
 * @param std::function<void(Object)>           handler
 * @return void
 */
void TdHandler::send_query(td_api::object_ptr<td_api::Function> f, 
                           std::function<void(Object)> handler)
{
    uint64_t query_id = next_query_id();

    if (handler)
        handlers_.emplace(query_id, std::move(handler));

    client_manager_->send(client_id_, query_id, std::move(f));
}


/**
 * @param timeout
 * @return void
 */
void TdHandler::loop(int timeout)
{
    if (need_restart_) {
        restart();
        return;
    }

    if (!are_authorized_) {
        process_response(client_manager_->receive(10));
        return;
    }

    handle_event_loop(timeout);
}


/**
 * @param timeout
 * @return bool
 */
void TdHandler::handle_event_loop(int timeout)
{
    while (true) {
        auto response = client_manager_->receive(timeout);

        if (response.object)
            process_response(std::move(response));
        else
            break;
    }
}


/**
 * @param td::ClientManager::Response response
 * @return void
 */
void TdHandler::process_response(td::ClientManager::Response response)
{
    if (!response.object)
        return;

    // std::cout
    //     << response.request_id
    //     << " "
    //     << to_string(response.object)
    //     << std::endl;

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
void TdHandler::process_update(td_api::object_ptr<td_api::Object> update)
{
    td_api::downcast_call(
        *update,
        overloaded(
            [this](td_api::updateAuthorizationState &update) {
                authorization_state_ = std::move(update.authorization_state_);
                on_authorization_state_update();
                callback.execute(update);
            },
            [this](td_api::updateNewChat &update) {
                chat_title_[update.chat_->id_] = update.chat_->title_;
                callback.execute(update);
            },
            [this](td_api::updateChatTitle &update) {
                chat_title_[update.chat_id_] = update.title_;
                callback.execute(update);
            },
            [this](td_api::updateUser &update) {
                users_[update.user_->id_] = std::move(update.user_);
                callback.execute(update);
            },
            [this](td_api::updateNewMessage &update) {
                callback.execute(update);
            },
            [](auto &update) {}
        )
    );
}


/**
 * @return void
 */
void TdHandler::on_authorization_state_update()
{
    on_auth_update_mutex.lock();
    authentication_query_id_++;
    td_api::downcast_call(
        *authorization_state_,
        overloaded(
            [this](td_api::authorizationStateReady &) {

                are_authorized_ = true;
                std::cout << "Got authorizationStateReady" << std::endl;

                auto callback = [this](Object obj){
                    if (obj->get_id() != td_api::user::ID)
                        return;
                    user_id_ = (td::move_tl_object_as<td_api::user>(obj))->id_;
                };

                /* Get account user_id. */
                send_query(td_api::make_object<td_api::getMe>(), callback);
            },
            [this](td_api::authorizationStateLoggingOut &) {
                are_authorized_ = false;
                std::cout << "Logging out" << std::endl;
            },
            [this](td_api::authorizationStateClosing &) {
                std::cout << "Closing" << std::endl;
            },
            [this](td_api::authorizationStateClosed &) {
                closed_         = true;
                need_restart_   = true;
                are_authorized_ = false;
                std::cout << "Terminated" << std::endl;
            },
            [this](td_api::authorizationStateWaitCode &) {
                std::cout << "Enter authentication code: " << std::flush;

                std::string code;
                std::getline(std::cin, code);
                send_query(
                    td_api::make_object<td_api::checkAuthenticationCode>(code),
                    create_authentication_query_handler()
                );
            },
            [this](td_api::authorizationStateWaitRegistration &) {
                std::string first_name;
                std::string last_name;

                std::cout << "Enter your first name: " << std::flush;
                std::getline(std::cin, first_name);
                std::cout << "Enter your last name: " << std::flush;
                std::getline(std::cin, last_name);

                send_query(
                    td_api::make_object<td_api::registerUser>(first_name, 
                                                              last_name),
                    create_authentication_query_handler()
                );
            },
            [this](td_api::authorizationStateWaitPassword &) {
                std::cout << "Enter authentication password: " << std::flush;
                std::string password;
                std::getline(std::cin, password);

                send_query(
                    td_api::make_object<td_api::checkAuthenticationPassword>(
                        password
                    ),
                    create_authentication_query_handler()
                );
            },
            [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state)
            {
                std::cout
                    << "Confirm this login link on another device: "
                    << state.link_
                    << std::endl;
            },
            [this](td_api::authorizationStateWaitPhoneNumber &) {
                std::cout << "Enter phone number: " << std::flush;
                std::string phone_number;
                std::getline(std::cin, phone_number);

                send_query(
                    td_api::make_object<td_api::setAuthenticationPhoneNumber>(
                        phone_number, nullptr),
                    create_authentication_query_handler()
                );
            },
            [this](td_api::authorizationStateWaitEncryptionKey &) {
                std::cout << "Enter encryption key or DESTROY: " << std::flush;
                std::string key;
                std::getline(std::cin, key);

                if (key == "DESTROY")
                    send_query(td_api::make_object<td_api::destroy>(),
                               create_authentication_query_handler());
                else
                    send_query(
                        td_api::make_object<td_api::checkDatabaseEncryptionKey>(
                            std::move(key)
                        ),
                        create_authentication_query_handler()
                    );
            },
            [this](td_api::authorizationStateWaitTdlibParameters &) {
                auto par = td_api::make_object<td_api::tdlibParameters>();

                par->use_message_database_ = true;
                par->use_secret_chats_ = false;
                par->api_id_ = this->api_id_;
                par->api_hash_ = this->api_hash_;
                par->database_directory_ = this->data_path_;
                par->system_language_code_ = "en";
                par->device_model_ = "Desktop";
                par->application_version_ = "1.0";
                par->enable_storage_optimizer_ = true;

                send_query(
                    td_api::make_object<td_api::setTdlibParameters>(
                        std::move(par)
                    ),
                    create_authentication_query_handler()
                );
            },
            [this](auto &){}
        )
    );
    on_auth_update_mutex.unlock();
}


/**
 *
 */
/**
 * @return std::function<void(Object object)>
 */
std::function<void(Object object)>
TdHandler::create_authentication_query_handler()
{
    return [this, id = authentication_query_id_.load()](Object object) {
        if (id == authentication_query_id_.load()) {
            check_authentication_error(std::move(object));
        }
    };
}

/**
 * @param Object object
 */
void TdHandler::check_authentication_error(Object object)
{
    if (object->get_id() == td_api::error::ID) {
        auto error = td::move_tl_object_as<td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;
        on_authorization_state_update();
    }
}


/**
 * @return void
 */
void TdHandler::close()
{
    send_query(td_api::make_object<td_api::close>(), {});
    std::cout << "Waiting for authorizationStateClosed..." << std::endl;
    handle_event_loop(5);
}

} /* namespace TdTeaBot */
