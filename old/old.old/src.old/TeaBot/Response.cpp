
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <iostream>
#include <TeaBot/Response.hpp>
#include <TeaBot/Responses/Message.hpp>
#include <TeaBot/Responses/MyMessage.hpp>

namespace TeaBot {

/**
 * @param td_api::updateNewMessage   update
 * @param std::shared_ptr<TdHandler> handler_
 */
Response::Response(td_api::updateNewMessage update,
		   std::shared_ptr<TdHandler> handler):
	update_(std::move(update)),
	handler_(handler)
{

	auto &update_ = this->update_;
	auto &message = update_.message_;
	auto &content = *(message->content_);

	switch (message->content_->get_id()) {
	case td_api::messageText::ID: {
		auto &text = static_cast<td_api::messageText &>(content).text_;
		text_ = text->text_;
		break;
	}

	}
}


Response::~Response()
{
}

	
/** 
 * @return void
 */
void Response::run()
{

	auto f1 = [this](td_api::messageSenderUser &user) {
		sender_id_   = user.user_id_;
		sender_name_ = handler_->get_user_name(sender_id_);
	};

	auto f2 = [this](td_api::messageSenderChat &chat) {
		sender_id_   = chat.chat_id_;
		sender_name_ = handler_->get_chat_title(sender_id_);
	};

	chat_id_ = update_.message_->chat_id_;
	td_api::downcast_call(*(update_.message_->sender_),
			      overloaded(std::move(f1), std::move(f2)));

	is_self_msg_ = (handler_->get_user_id() == sender_id_);

	if (is_self_msg_) {
		Responses::MyMessage res(self_);
		res.run();
	} else {
		Responses::Message res(self_);
		res.run();
	}
}


} /* namespace TeaBot */
