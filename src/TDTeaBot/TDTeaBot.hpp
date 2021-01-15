
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TDTeaBot
 */

#ifndef TDTEABOT__TDTEABOT_HPP
#define TDTEABOT__TDTEABOT_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace td_api = td::td_api;

namespace TDTeaBot {

class TDTeaBot
{
private:
    uint32_t api_id_;
    const char *api_hash_  = NULL;
    const char *data_path_ = NULL;

public:
    TDTeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
    ~TDTeaBot();
};

} /* namespace TDTeaBot */

#endif /* #ifndef TDTEABOT__TDTEABOT_HPP */
