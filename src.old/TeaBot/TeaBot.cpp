
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#include "TeaBot.hpp"
#include "Responses.hpp"

#include <cstdio>
#include <unistd.h>
#include <signal.h>

namespace TeaBot {

bool is_signaled       = false;
bool is_sighandler_set = false;
pthread_mutex_t sigmut = PTHREAD_MUTEX_INITIALIZER;

static void *teabot_thread(void *p_);


/**
 * @param int sig
 * @return void
 */
static void teabot_sighandler(int sig)
{
    is_signaled = true;
    printf("\nGot an interrupt signal!\n");
    fflush(stdout);
}


/**
 * @param uint32_t   api_id
 * @param const char *api_hash
 * @param const char *data_path
 *
 * Constructor.
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
        handler_(api_id, api_hash, data_path)
    ,   nthread(0)
{

    pthread_mutex_lock(&sigmut);
    __sync_synchronize();
    if (!is_sighandler_set) {
        signal(SIGINT, teabot_sighandler);
        signal(SIGHUP, teabot_sighandler);
        signal(SIGTERM, teabot_sighandler);
        is_sighandler_set = true;
    }
    pthread_mutex_unlock(&sigmut);

    handler_.setCallback(
        [this](td_api::updateNewMessage &update){
            this->onUpdateNewMessage(update);
        }
    );
}


/**
 * Destructor.
 */
TeaBot::~TeaBot()
{
    handler_.close();
}


/**
 * @return void
 */
void TeaBot::run()
{
    while (true) {

        if (is_signaled)
            break;

        handler_.loop();
    }
}


/**
 * @param td_api::updateNewMessage &update
 * @return void
 */
void TeaBot::onUpdateNewMessage(td_api::updateNewMessage &update)
{
    spawn_response_handler_thread(update);
}


/**
 * @param void *p_
 * @return void *
 */
static void *response_thread(void *p_)
{
    Responses *res = (Responses *)p_;
    res->run();

    delete res;
    return NULL;
}


/**
 * @param td_api::updateNewMessage &update
 * @return void
 */
void TeaBot::spawn_response_handler_thread(td_api::updateNewMessage &update)
{
    Responses *res;
    pthread_t thread;

    res = new Responses(std::move(update), &handler_);
    pthread_create(&thread, NULL, response_thread, (void *)res);
    pthread_detach(thread);
}

} /* namespace TeaBot */