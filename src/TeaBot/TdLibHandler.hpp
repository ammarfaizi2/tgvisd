
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#ifndef __TdLibHandler_HPP
#define __TdLibHandler_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <string>
#include <memory>
#include <cstdint>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "detail.hpp"

namespace td_api = td::td_api;

namespace TeaBot {

class TdLibHandler
{
private:
    uint32_t    api_id_;
    const char  *api_hash_;
    const char  *data_path_;

    bool closed_{false};
    bool are_authorized_{false};
    bool need_restart_{false};

    int32_t client_id_{0};
    int32_t user_id_{0};
    uint64_t current_query_id_{0};
    uint64_t authentication_query_id_{0};

    using Object = td_api::object_ptr<td_api::Object>;

    std::unique_ptr<td::ClientManager> client_manager_;
    td_api::object_ptr<td_api::AuthorizationState> authorization_state_;

    std::unordered_map<int64_t, std::string> chat_title_;
    std::unordered_map<uint64_t, std::function<void(Object)>> handlers_;
    std::unordered_map<int32_t, td_api::object_ptr<td_api::user>> users_;

    std::function<void(td_api::updateNewMessage &update, TdLibHandler *handler)>
        onUpdateNewMessageCallback;

    void restart();
    bool handle_event_loop();
    uint64_t next_query_id();
    void on_authorization_state_update();
    void check_authentication_error(Object object);
    void process_response(td::ClientManager::Response response);
    void process_update(td_api::object_ptr<td_api::Object> update);
    std::function<void(Object object)> create_authentication_query_handler();

public:
    TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path);

    void loop();
    void close();
    int32_t get_client_id();
    int32_t get_user_id();

    std::string get_user_name(int32_t user_id) const;
    std::string get_chat_title(int64_t chat_id) const;

    void send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler);
    void setCallback(
        std::function<void(td_api::updateNewMessage &update, TdLibHandler *handler)>
        onUpdateNewMessageCallback
    );
};

} /* namespace TeaBot */

#endif /* #ifndef __TdLibHandler_HPP */
