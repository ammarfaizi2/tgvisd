
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TdTeaBot
 */

#ifndef TEABOT__TEABOT_HPP
#define TEABOT__TEABOT_HPP

#include <TdTeaBot/TdHandler.hpp>

using TdTeaBot::TdHandler;

namespace TeaBot {

class TeaBot
{
private:    
    std::shared_ptr<TdHandler> handler_;

public:
    TeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
    ~TeaBot();
};

} /* namespace TeaBot */

#endif /* #ifndef TEABOT__TEABOT_HPP */
