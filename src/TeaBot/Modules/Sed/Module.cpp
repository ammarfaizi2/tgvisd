
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */
#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/Sed/Module.hpp>

namespace TeaBot::Modules::Sed {

/**
 * @return bool
 */
bool Module::check_replied_msg()
{
    std::mutex  mut;
    bool        action = false;

    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto getMsg    = td_api::make_object<td_api::getMessage>(
        msg->reply_in_chat_id_,
        msg->reply_to_message_id_
    );

    std::string &replied_text_ = this->replied_text_;

    mut.lock();
    res_->handler_->send_query(
        std::move(getMsg),
        [&action, &replied_text_, &mut] (auto object) {

            if (object->get_id() == td_api::error::ID)
                goto ret;

            {
                auto msg = td::move_tl_object_as<td_api::message>(object);
                if (msg->content_->get_id() != td_api::messageText::ID)
                    goto ret; /* Skip non text message. */

                action         = true;
                auto &c        = msg->content_;
                auto &rtext    = static_cast<td_api::messageText &>(*c).text_;
                replied_text_  = std::move(rtext->text_);
            }

        ret:
            mut.unlock();
        }
    );
    mut.lock();

    return action;
}

/**
 * @param const char *pat
 * @param size_t     len
 * @return void
 */
#define PRINT_CMD "printf \"%s\" "
#define SED_CMD   " | sed -r "
void Module::run(const char *pat, size_t len)
{
    if (!check_replied_msg())
        return;

    size_t alloc = 0; /* Heap allocation size. */
    size_t esc_len = 0;

    const char *crep      = replied_text_.c_str();
    size_t      crep_len  = replied_text_.size();

    alloc += (crep_len * 4) + 1; /* (escaped) Replied message allocation. */
    alloc += (len * 4) + 4;      /* (escaped) Pattern allocation. */
    alloc += sizeof(PRINT_CMD) + sizeof(SED_CMD) + sizeof(" 2>&1");

    char *heap = new char[alloc];
    char *cmd  = heap;

    memcpy(cmd, PRINT_CMD, sizeof(PRINT_CMD) - 1);
    cmd += sizeof(PRINT_CMD) - 1;
    escapeshellarg(cmd, crep, crep_len, &esc_len);
    cmd += esc_len;
    memcpy(cmd, SED_CMD, sizeof(SED_CMD) - 1);
    cmd += sizeof(SED_CMD) - 1;
    escapeshellarg(cmd, pat, len, &esc_len);
    cmd += esc_len;
    memcpy(cmd, " 2>&1", sizeof(" 2>&1"));

    std::string reply = "Sed output:\n\n";
    reply.reserve(16 + (crep_len * 2) + 4096);
    reply += shell_exec(heap);
    delete[] heap;

    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto imt       = td_api::make_object<td_api::inputMessageText>();

    /* Text formatting. */
    auto pre       = td_api::make_object<td_api::textEntityTypeBold>();
    auto prer      = td::move_tl_object_as<td_api::TextEntityType>(pre);
    auto text_ent  = td_api::make_object<td_api::textEntity>(0, 11,
                        std::move(prer));
    auto entities  = std::vector<decltype(text_ent)>();
    entities.push_back(std::move(text_ent));
    auto text      = td_api::make_object<td_api::formattedText>(reply,
                        std::move(entities));
    imt->text_     = std::move(text);

    auto rmsg         = td_api::make_object<td_api::sendMessage>();
    rmsg->chat_id_    = res_->chat_id_;
    rmsg->reply_to_message_id_ = msg->reply_to_message_id_;
    rmsg->input_message_content_ = std::move(imt);
    res_->handler_->send_query(std::move(rmsg), {});
}

} /* namespace TeaBot::Modules::Sed */

