
#ifndef __TeaBot__TeaBot_HPP
#define __TeaBot__TeaBot_HPP

namespace TeaBot {

class TeaBot
{
private:
  const uint32_t api_id_;
  const char     *api_hash_;
  const char     *data_path_;

public:
  TeaBot(uint32_t api_id, const char *api_hash, const char *data_path_);

  ~TeaBot();

  void run();
};

} /* namespace TeaBot */

#endif /* #ifndef __TeaBot__TeaBot_HPP */
