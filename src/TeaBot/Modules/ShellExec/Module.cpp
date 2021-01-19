
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

#define SHELL_WRAP_STR "exec /usr/bin/bash -c "

/**
 * @param const char *cmd
 * @param size_t     len
 * @return void
 */
void Module::run(const char *cmd, size_t len)
{
    size_t escaped_len  = 0;
    size_t wrapped_len  = sizeof(SHELL_WRAP_STR) + (len * 4) + sizeof(" 2>&1");
    char   *wrapped_cmd = new char[wrapped_len];
    char   *comp_ptr    = wrapped_cmd;

    memcpy(comp_ptr, SHELL_WRAP_STR, sizeof(SHELL_WRAP_STR) - 1);
    comp_ptr += sizeof(SHELL_WRAP_STR) - 1;
    comp_ptr  = escapeshellarg(comp_ptr, cmd, len, &escaped_len);
    comp_ptr  = trim_len_cpy(comp_ptr, escaped_len, &escaped_len);
    comp_ptr += escaped_len;
    memcpy(comp_ptr, " 2>&1", 6);

    std::string cmd_out = std::move(shell_exec(wrapped_cmd));
    delete[] wrapped_cmd;

    size_t out_len = cmd_out.size();

    if (out_len == 0) {
        cmd_out = "~";
    }

    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto emsg      = td_api::make_object<td_api::editMessageText>();
    auto imt       = td_api::make_object<td_api::inputMessageText>();

    /* Text formatting. */
    auto pre       = td_api::make_object<td_api::textEntityTypePreCode>();
    auto prer      = td::move_tl_object_as<td_api::TextEntityType>(pre);
    auto text_ent  = td_api::make_object<td_api::textEntity>(0, out_len,
                        std::move(prer));
    auto entities  = std::vector<decltype(text_ent)>();
    entities.push_back(std::move(text_ent));
    auto text      = td_api::make_object<td_api::formattedText>(cmd_out,
                        std::move(entities));

    imt->text_        = std::move(text);
    emsg->chat_id_    = res_->chat_id_;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);
    res_->handler_->send_query(std::move(emsg), {});
}

} /* namespace TeaBot::Modules::ShellExec */
