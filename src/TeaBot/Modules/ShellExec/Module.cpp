
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/ShellExec/Module.hpp>

namespace TeaBot::Modules::ShellExec {

/**
 * @return const char *
 */
inline static std::string shell_exec(const char *cmd)
{
    char    out[8096];
    size_t  outlen  = 0;
    FILE    *handle = popen(cmd, "r");
    if (handle == NULL) {
        outlen = (size_t)snprintf(out, sizeof(out), "Error: %s",
                                  strerror(errno));
        goto ret;
    }
    outlen = fread(out, sizeof(char), sizeof(out), handle);
    pclose(handle);
ret:
    {
        std::string ret;
        ret.assign(out, outlen);
        return ret;
    }
}

#define SHELL_WRAP_STR "exec /usr/bin/bash -c "

/**
 * @param const char *cmd
 * @param size_t     len
 * @return void
 */
void Module::run(const char *cmd, size_t len)
{
    size_t wraped_len   = (len * 4) + sizeof(SHELL_WRAP_STR);
    char   *wrapped_cmd = new char[wraped_len + sizeof("2>&1") + 1];
    char   *escaped_cmd = new char[len * 4];

    strcpy(wrapped_cmd, SHELL_WRAP_STR);
    strcat(wrapped_cmd, escapeshellarg(escaped_cmd, cmd, len));
    strcat(wrapped_cmd, " 2>&1");

    delete[] escaped_cmd;
    std::string cmd_output = std::move(shell_exec(wrapped_cmd));
    delete[] wrapped_cmd;

    auto &update_ = res_->update_;
    td_api::object_ptr<td::td_api::message> &msg = update_.message_;

    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_        = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = std::move(cmd_output);
    emsg->chat_id_    = res_->chat_id_;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    res_->handler_->send_query(std::move(emsg), {});
}

} /* namespace TeaBot::Modules::ShellExec */
