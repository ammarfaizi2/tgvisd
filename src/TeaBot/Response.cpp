
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <iostream>
#include <TeaBot/Response.hpp>

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
}


Response::~Response()
{
}


/** 
 * @return void
 */
void Response::run()
{
    std::cout << to_string(update_) << std::endl;
}


} /* namespace TeaBot */
