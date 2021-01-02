
#ifndef __TeaBot_HPP
#define __TeaBot_HPP

#include "TdLibHandler.hpp"

namespace TeaBot {

class TeaBot
{
private:
  TdLibHandler handler;
  const char *storage_path_;


public:
  TeaBot(uint32_t api_id, const char *api_hash, const char *storage_path);

  void
  onUpdateNewMessage(
    td_api::updateNewMessage &update,
    TdLibHandler *handler
  );

  ~TeaBot();

  void
  run();
};

} /* namespace TeaBot */

#endif
