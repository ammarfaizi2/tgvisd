
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#ifndef __TeaBot__TeaBot_HPP
#define __TeaBot__TeaBot_HPP

#include <cstdint>
#include <pthread.h>

#define TEABOT_NUM_OF_THREADS (5)
#define TEABOT_NUM_OF_QUEUES  (10)

#include "TdLibHandler.hpp"

namespace TeaBot {

class TeaBot
{
private:
    size_t nthread;
    TdLibHandler handler_;

    void spawn_response_handler_thread(td_api::updateNewMessage &update);

public:
    TeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
    ~TeaBot();

    void run();
    void onUpdateNewMessage(td_api::updateNewMessage &update);
};

} /* namespace TeaBot */

#endif /* #ifndef __TeaBot__TeaBot_HPP */
