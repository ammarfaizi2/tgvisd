
#include "TdLibHandler.hpp"


namespace TeaBot {


/**
 * @param uint32_t    api_id
 * @param const char  *api_hash
 * @param const char  *data_path
 *
 * Constructor.
 */
TdLibHandler::TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path):
    api_id_(api_id)
  , api_hash_(api_hash)
  , data_path_(data_path_)
{
    td::ClientManager::execute(
        td_api::make_object<td_api::setLogVerbosityLevel>(1));

    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_      = client_manager_->create_client_id();
    send_query(td_api::make_object<td_api::getOption>("version"), {});
}


/**
 * @return void
 */
void TdLibHandler::restart()
{
    client_manager_.reset();
    *this = TdLibHandler(api_id_, api_hash_, data_path_);
}


/**
 * @param td_api::object_ptr<td_api::Function>  f
 * @param std::function<void(Object)>           handler
 * @return void
 */
void TdLibHandler::send_query(td_api::object_ptr<td_api::Function> f, 
                              std::function<void(Object)> handler)
{
    auto query_id = next_query_id();
    if (handler)
        handlers_.emplace(query_id, std::move(handler));

    client_manager_->send(client_id_, query_id, std::move(f));
}

/**
 * @return uint64_t
 */
uint64_t TdLibHandler::next_query_id()
{
    return ++current_query_id_;
}

} /* namespace TeaBot */
