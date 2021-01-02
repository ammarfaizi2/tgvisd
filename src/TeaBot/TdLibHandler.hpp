
#ifndef __TdLibHandler_HPP
#define __TdLibHandler_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <map>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <functional>

#include "detail.hpp"

namespace td_api = td::td_api;

using Object = td_api::object_ptr<td_api::Object>;

namespace TeaBot {

class TdLibHandler
{
private:
  uint32_t    api_id_;
  const char  *api_hash_;
  const char  *data_path_;

  std::unique_ptr<td::ClientManager> client_manager_;
  td_api::object_ptr<td_api::AuthorizationState> authorization_state_;

  void restart();

public:
  TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path);
  ~TdLibHandler();

  void loop();
};

} /* namespace TeaBot */

#endif /* #ifndef __TdLibHandler_HPP */
