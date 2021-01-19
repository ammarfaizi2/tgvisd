
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

#endif /* #ifndef TEABOT__HELPERS_HPP */
