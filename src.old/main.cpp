
#include "TeaBot.hpp"

/**
 * @param int  argc
 * @param char *argv[]
 */
int main(int argc, char *argv[])
{

  if (argc != 2) {
    printf("Usage: %s [account]\n", argv[0]);
    return 0;
  }

  const uint32_t api_id    = 2841585;
  const char     *api_hash = "fed2843d34aeb38d86cee964243d6ed9";

  TeaBot::TeaBot bot(api_id, api_hash, argv[1]);
  bot.run();

  return 0;
}
