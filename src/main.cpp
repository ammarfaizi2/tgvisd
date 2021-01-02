
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

  TeaBot::TeaBot bot(argv[1]);
  bot.run();

  return 0;
}
