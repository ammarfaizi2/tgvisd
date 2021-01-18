
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TdTeaBot
 */

#ifndef TDTEABOT__TDTEABOT_HPP
#define TDTEABOT__TDTEABOT_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <TdTeaBot/detail.hpp>

namespace td_api = td::td_api;

#include <TdTeaBot/Callback.hpp>

namespace TdTeaBot {

using Object = td_api::object_ptr<td_api::Object>;

class TdHandler
{
private:
    uint32_t    api_id_;
    const char  *api_hash_;
    const char  *data_path_;

    bool closed_{false};
    bool need_restart_{false};
    bool are_authorized_{false};

    int32_t user_id_{0};
    int32_t client_id_{0};
    std::atomic<uint64_t> current_query_id_{0};
    std::atomic<uint64_t> authentication_query_id_{0};

    using Object = td_api::object_ptr<td_api::Object>;

    std::unique_ptr<td::ClientManager> client_manager_;
    td_api::object_ptr<td_api::AuthorizationState> authorization_state_;

    std::unordered_map<int64_t, std::string> chat_title_;
    std::unordered_map<uint64_t, std::function<void(Object)>> handlers_;
    std::unordered_map<int32_t, td_api::object_ptr<td_api::user>> users_;

    std::mutex on_auth_update_mutex;

    void restart();
    uint64_t next_query_id();
    void handle_event_loop();
    void on_authorization_state_update();
    void check_authentication_error(Object object);
    void process_response(td::ClientManager::Response response);
    void process_update(td_api::object_ptr<td_api::Object> update);
    std::function<void(Object object)> create_authentication_query_handler();

public:
    Callback callback;

    TdHandler(uint32_t api_id, const char *api_hash, const char *data_path);

    void loop();
    void close();
    void send_query(td_api::object_ptr<td_api::Function> f,
                    std::function<void(Object)> handler);
};

} /* namespace TdTeaBot */

#endif /* #ifndef TDTEABOT__TDTEABOT_HPP */
