
#ifndef __TeaBot_HPP
#define __TeaBot_HPP

#include "Responses.hpp"
#include "TdLibHandler.hpp"

namespace TeaBot8 {

class TeaBot
{
private:
  TdLibHandler handler;
  const char *storage_path_;


public:
  TeaBot(const char *storage_path);

  ~TeaBot();

  void
  run();
};

} /* namespace TeaBot8 */

#endif
