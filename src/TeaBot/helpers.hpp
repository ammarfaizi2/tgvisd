
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__HELPERS_HPP
#define TEABOT__HELPERS_HPP

#include <string>

char *escapeshellarg(char *alloc, const char *str, size_t len, size_t *res_len);
std::string shell_exec(const char *cmd);
char *trim_len(char *head, size_t len, size_t *res_len);
char *trim_len_cpy(char *str, size_t len, size_t *res_len);
char *trim(char *str);
char *trim_cpy(char *str);


/**
 * @param char x
 * @return char
 */
inline static char tolower(char x)
{
  return ('A' <= x && x <= 'Z') ? x + 32 : x;
}


/**
 * @param char x
 * @return char
 */
inline static char toupper(char x)
{
  return ('a' <= x && x <= 'z') ? x - 32 : x;
}


/**
 * @param char   *str
 * @param size_t len
 * @return void
 */
inline static void mock_string(char *str, size_t len)
{
  bool c = true;
  while (len--) {
    *str = c ? tolower(*str) : toupper(*str);
    str++;
    c = !c;
  }
}

#endif /* #ifndef TEABOT__HELPERS_HPP */
