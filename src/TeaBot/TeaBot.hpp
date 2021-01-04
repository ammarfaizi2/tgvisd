
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license MIT
 * @package \TeaBot
 */

#ifndef __TeaBot__TeaBot_HPP
#define __TeaBot__TeaBot_HPP

#include <cstdint>

#include "TdLibHandler.hpp"

namespace TeaBot {

class TeaBot
{
private:
  TdLibHandler handler_;

public:
  TeaBot(uint32_t api_id, const char *api_hash, const char *data_path);
  ~TeaBot();

  void run();
  void onUpdateNewMessage(td_api::updateNewMessage &update, TdLibHandler *handler);
};

} /* namespace TeaBot */

#endif /* #ifndef __TeaBot__TeaBot_HPP */
