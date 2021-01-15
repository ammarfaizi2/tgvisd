
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TDTeaBot
 */

#ifndef TEABOT__TEABOT_HPP
#define TEABOT__TEABOT_HPP

#include <TDTeaBot/TDTeaBot.hpp>

using TDTeaBot::TDTeaBot;

namespace TeaBot {

class TeaBot
{
private:    
    TDTeaBot tdtb;
public:
    TeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
    ~TeaBot();
};

} /* namespace TeaBot */

#endif /* #ifndef TEABOT__TEABOT_HPP */
