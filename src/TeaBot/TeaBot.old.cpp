
#include "TeaBot.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

namespace TeaBot {

static void handleSelfMessage(td_api::updateNewMessage &update, TdLibHandler *handler);

/**
 * @param uint32_t   api_id
 * @param const char *api_hash
 * @param const char *data_path
 *
 * Constructor.
 */
TeaBot::TeaBot(uint32_t api_id, const char *api_hash, const char *data_path):
    handler_(api_id, api_hash, data_path)
{
    handler_.setCallback([this](td_api::updateNewMessage &update, TdLibHandler *handler){
       this->onUpdateNewMessage(update, handler);
    });
}


/**
 * Destructor.
 */
TeaBot::~TeaBot()
{
    handler_.close();
}

static bool is_signaled = false;

static void handle_signal(int sig)
{
    std::cout << "\nGot interrupt signal!" << std::endl;
    is_signaled = true;
}

/**
 * @return void
 */
void TeaBot::run()
{
    signal(SIGINT, handle_signal);
    while (true) {
        if (is_signaled) {
            break;
        }
        handler_.loop();
    }
}


typedef struct _thread_data
{
    TdLibHandler *handler;
    td_api::updateNewMessage update;
} thread_data;


static void *thread_handler(void *_p)
{
    thread_data *data = (thread_data *)_p;
    handleSelfMessage(data->update, data->handler);
    delete data;
    return NULL;
}


/**
 * @param td_api::updateNewMessage &update
 * @param TdLibHandler             *handler
 * @return void
 */
void TeaBot::onUpdateNewMessage(td_api::updateNewMessage &update, TdLibHandler *handler)
{
    // 1472415329
    // std::cout << to_string(update) << std::endl;

    std::string sender_name;

    int64_t sender_id  = 0;
    int64_t chat_id    = update.message_->chat_id_;
    int64_t my_user_id = handler->get_user_id();

    td_api::downcast_call(
        *update.message_->sender_,
        overloaded(
            [&](td_api::messageSenderUser &user) {
                sender_id   = user.user_id_;
                sender_name = handler->get_user_name(sender_id);
            },
            [&](td_api::messageSenderChat &chat) {
                sender_id   = chat.chat_id_;
                sender_name = handler->get_chat_title(sender_id);
            }
        )
    );

    std::cout << to_string(update) << std::endl;
    if (my_user_id == sender_id) {
        pthread_t thread;
        thread_data *data = new thread_data;

        data->update  = std::move(update);
        data->handler = handler;
        pthread_create(&thread, NULL, thread_handler, (void *)data);
        pthread_detach(thread);
    }
}


/**
 * @return const char *
 */
static std::string shell_exec(const char *cmd)
{
    char *p;
    char out[10240];
    FILE *handle;

    handle = popen(cmd, "r");
    if (handle == NULL) {
        sprintf(out, "Error: %s", strerror(errno));
        goto ret;
    }

    p = out;
    while (fgets(p, 4096, handle) != NULL) {
        p += strlen(p);
    }

    pclose(handle);

ret:
    return std::string(out);
}


/**
 * @return void
 */
static void handleSelfMessage(td_api::updateNewMessage &update, TdLibHandler *handler)
{
    if (update.message_->content_->get_id() != td_api::messageText::ID) {
        return;
    }

    const char *cmd;
    auto &msg = update.message_;
    int64_t chat_id = msg->chat_id_;
    std::string text;

    text = static_cast<td_api::messageText &>(*update.message_->content_).text_->text_;
    cmd  = text.c_str();

    if (strncmp(cmd, "!sh ", 4) != 0) {
        return;
    }
    cmd += 4;

    if (strlen(cmd) == 0) {
        return;
    }

    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_ = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = shell_exec(cmd);
    emsg->chat_id_    = chat_id;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    handler->send_query(std::move(emsg), {});
}


} /* namespace TeaBot */
