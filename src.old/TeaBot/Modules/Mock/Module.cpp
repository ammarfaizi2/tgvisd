
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Mock
 */

#include <iostream>
#include <cstdio>
#include <errno.h>
#include <TeaBot/helpers.hpp>
#include <TeaBot/Modules/Mock/Module.hpp>

namespace TeaBot::Modules::Mock {

/**
 * @return bool
 */
bool Module::check_replied_msg()
{
	std::mutex  mut;
	bool action = false;

	auto &update_	= res_->update_;
	auto &msg	= update_.message_;
	auto getMsg	= td_api::make_object<td_api::getMessage>(
		msg->reply_in_chat_id_,
		msg->reply_to_message_id_
	);

	std::string &replied_text_ = this->replied_text_;

	mut.lock();
	auto callback = [&action, &replied_text_, &mut] (auto object) {

		td::tl_object_ptr<td_api::message> msg;

		if (object->get_id() == td_api::error::ID)
			goto out;

		msg = td::move_tl_object_as<td_api::message>(object);
		if (msg->content_->get_id() != td_api::messageText::ID)
			goto out; /* Skip non text message. */


		{
			action		= true;
			auto &c		= msg->content_;
			auto &rtext	= static_cast<td_api::messageText &>
					    (*c).text_;
			replied_text_	= std::move(rtext->text_);
		}
	out:
		mut.unlock();
	};
	res_->handler_->send_query(std::move(getMsg), std::move(callback));
	mut.lock();

	return action;
}

/**
 * @return void
 */
void Module::run()
{
	if (!check_replied_msg())
		return;

	const char *crep = replied_text_.c_str();
	size_t crep_len = replied_text_.size();

	mock_string((char *)crep, crep_len);

	auto &update_	= res_->update_;
	auto &msg	= update_.message_;
	auto imt	= td_api::make_object<td_api::inputMessageText>();
	auto text	= td_api::make_object<td_api::formattedText>();
	text->text_	= std::move(replied_text_);
	imt->text_	= std::move(text);

	auto rmsg	= td_api::make_object<td_api::editMessageText>();
	rmsg->chat_id_	= res_->chat_id_;
	rmsg->message_id_ = msg->id_;
	rmsg->input_message_content_ = std::move(imt);
	res_->handler_->send_query(std::move(rmsg), {});
}

} /* namespace TeaBot::Modules::Mock */

