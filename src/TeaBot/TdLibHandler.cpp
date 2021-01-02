
#include "TdLibHandler.hpp"

namespace TeaBot {

/**
 * @param uint32_t   api_id
 * @param const char *api_hash
 * @param const char *data_path
 *
 * Constructor.
 */
TdLibHandler::TdLibHandler(uint32_t api_id, const char *api_hash, const char *data_path):
    api_id_(api_id)
  , api_hash_(api_hash)
  , data_path_(data_path)
{

}

/**
 * Destructor.
 */
TdLibHandler::~TdLibHandler()
{
}


/**
 * @return void
 */
void loop()
{
  while (true) {
    if (need_restart_) {
      restart();
      continue;
    }

    if (!are_authorized_) {
      process_response(client_manager_->receive(10));
      continue;
    }

    if (!handle_loop()) {
      break;
    }
  }
}


/**
 * @return void
 */
void TdLibHandler::restart()
{
  *this = std::move(TdLibHandler(api_id_, api_hash_, data_path_));
}


} /* namespace TeaBot */
