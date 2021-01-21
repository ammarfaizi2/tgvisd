
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::KernelUpdate
 */

#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/TeaCurl.hpp>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/KernelUpdate/Module.hpp>

#include <nlohmann/single_include/json.hpp>

using json = nlohmann::json;

namespace TeaBot::Modules::KernelUpdate {

/**
 * @return void
 */
void Module::run()
{
    char   *ret = NULL;

    std::string reply;

    reply.reserve(4096);

    try {
        TeaBot::TeaCurl cr("https://www.kernel.org/releases.json");
        cr.execute();

        ret    = cr.get_output();
        auto j = json::parse(ret);

        auto stable = j["latest_stable"].value("version", "unknown");
        auto latest = j["/releases/0"_json_pointer];

        reply  = "[Linux Kernel Release Info]\n";
        reply += "Latest stable: "+stable+"\n";
        reply += "Latest release: "
                 +latest.value("version", "unknown")
                 +" ("+latest.value("moniker", "???")+")\n";
        reply += "Scraped at: "+shell_exec("date -u")+"\n\n";
        reply += "See detailed info here: https://www.kernel.org/releases.json";
    } catch (const std::exception &e) {
        reply = std::string("Error: ")+e.what();
    }
    free(ret);

    auto &update_  = res_->update_;
    auto &msg      = update_.message_;
    auto imt       = td_api::make_object<td_api::inputMessageText>();
    auto text      = td_api::make_object<td_api::formattedText>();
    text->text_    = std::move(reply);
    imt->text_     = std::move(text);

    auto rmsg         = td_api::make_object<td_api::sendMessage>();
    rmsg->chat_id_    = res_->chat_id_;
    rmsg->reply_to_message_id_ = msg->id_;
    rmsg->input_message_content_ = std::move(imt);
    res_->handler_->send_query(std::move(rmsg), {});
}

} /* namespace TeaBot::Modules::KernelUpdate */
