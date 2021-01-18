
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TdTeaBot
 */

#ifndef TDTEABOT__CALLBACK_HPP
#define TDTEABOT__CALLBACK_HPP

namespace TdTeaBot {

class Callback
{
public:
    std::function<void(td_api::updateAuthorizationState &update)>
        updateAuthorizationState = nullptr;
    inline void execute(td_api::updateAuthorizationState &update)
    {
        updateAuthorizationState(update);
    }

    std::function<void(td_api::updateNewChat &update)>
        updateNewChat = nullptr;
    inline void execute(td_api::updateNewChat &update)
    {
        updateNewChat(update);
    }

    std::function<void(td_api::updateChatTitle &update)>
        updateChatTitle = nullptr;
    inline void execute(td_api::updateChatTitle &update)
    {
        updateChatTitle(update);
    }

    std::function<void(td_api::updateUser &update)>
        updateUser = nullptr;
    inline void execute(td_api::updateUser &update)
    {
        updateUser(update);
    }

    std::function<void(td_api::updateNewMessage &update)>
        updateNewMessage = nullptr;
    inline void execute(td_api::updateNewMessage &update)
    {
        updateNewMessage(update);
    }
};

} /* namespace TdTeaBot */

#endif
