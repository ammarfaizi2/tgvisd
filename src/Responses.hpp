
#ifndef __Responses_HPP
#define __Responses_HPP

#include <memory>

#include "TeaBot.hpp"

namespace TeaBot8 {

class Responses
{
private:
  using TeaBot = TeaBot;
  std::shared_ptr<TeaBot> bot_;

public:
  Responses(std::shared_ptr<TeaBot> bot_);
};

} /* namespace TeaBot8 */

#endif /* #ifndef __Responses_HPP */
