
#include "TdLibHandler.hpp"

namespace TeaBot {

RDTea::RDTea()
{
  td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(1));
  
  client_manager_ = std::make_unique<td::ClientManager>();
  client_id_      = client_manager_->create_client_id();

  send_query(td_api::make_object<td_api::getOption>("version"), {});
}


} /* namespace TeaBot */
