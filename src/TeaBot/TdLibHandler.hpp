
#ifndef __TdLibHandler_HPP
#define __TdLibHandler_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <map>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <functional>

#include "detail.hpp"

namespace td_api = td::td_api;

namespace TeaBot {

class TdLibHandler
{
private:
    uint32_t    api_id_;
    const char  *api_hash_;
    const char  *data_path_;

    bool are_authorized_{false};
    bool need_restart_{false};

    int32_t client_id_{0};
    uint64_t current_query_id_{0};
    uint64_t authentication_query_id_{0};

    using Object = td_api::object_ptr<td_api::Object>;

    std::unique_ptr<td::ClientManager> client_manager_;
    td_api::object_ptr<td_api::AuthorizationState> authorization_state_;

    std::map<int64_t, std::string> chat_title_;
    std::map<uint64_t, std::function<void(Object)>> handlers_;
    std::map<int32_t, td_api::object_ptr<td_api::user>> users_;

    void restart();
    uint64_t next_query_id();
public:
    TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path);

    void send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler);
};

} /* namespace TeaBot */

#endif /* #ifndef __TdLibHandler_HPP */
