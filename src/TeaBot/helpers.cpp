
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <TeaBot/helpers.hpp>

/** 
 * @param char       *alloc
 * @param const char *str
 * @retur char *
 */
char *escapeshellarg(char *alloc, const char *str, size_t len, size_t *res_len)
{
    size_t y = 0;
    size_t l = (len > 0) ? len : strlen(str);
    size_t x;
    char   *cmd;

    if (alloc == NULL) {
        cmd = (char *)malloc(sizeof(char) * l * 4); /* Worst case */
    } else {
        cmd = alloc;
    }

#ifdef WIN32
    cmd[y++] = '"';
#else
    cmd[y++] = '\'';
#endif

    for (x = 0; x < l; x++) {
        switch (str[x]) {
#ifdef WIN32
            case '"':
            case '%':
            case '!':
                cmd[y++] = ' ';
                break;
#else
            case '\'':
                cmd[y++] = '\'';
                cmd[y++] = '\\';
                cmd[y++] = '\'';
#endif
            /* fall-through */
            default:
                cmd[y++] = str[x];
        }
    }

#ifdef WIN32
    if (y > 0 && '\\' == cmd[y - 1]) {
        int k = 0, n = y - 1;
        for (; n >= 0 && '\\' == cmd[n]; n--, k++);
        if (k % 2) {
            cmd[y++] = '\\';
        }
    }
    cmd[y++] = '"';
#else
    cmd[y++] = '\'';
#endif

    cmd[y] = '\0';

    if (res_len != NULL)
        *res_len = y;

    return cmd;
}


/**
 * @param char *str
 * @param size_t len
 * @param size_t *res_ken
 * @return char *
 */
char *trim_len(char *str, size_t len, size_t *res_len)
{
    bool   move = false;
    char   *end = &(str[len - 1]);

    while (*str == ' ' || *str == '\n' || *str == '\r') {
        str++;
        len--;
        move = true;
    }

    while (*end == ' ' || *end == '\n' || *end == '\r') {
        end--;
        len--;
    }

    if (res_len != NULL)
        *res_len = len;

    if (move)
        *(end + 1) = '\0';

    return str;
}


/**
 * @param char *str
 * @param size_t len
 * @param size_t *res_ken
 * @return char *
 */
char *trim_len_cpy(char *str, size_t len, size_t *res_len)
{
    bool   move   = false;
    char   *end   = &(str[len - 1]);
    char   *start = str;

    while (*str == ' ' || *str == '\n' || *str == '\r') {
        str++;
        len--;
        move = true;
    }

    while (*end == ' ' || *end == '\n' || *end == '\r') {
        end--;
        len--;
    }

    if (res_len != NULL)
        *res_len = len;

    if (move) {
        *(start + len) = '\0';
        return (char *)memmove(start, str, len);
    }

    return start;
}


/**
 * @param char *str
 */
char *trim(char *str)
{
    return trim_len(str, strlen(str), NULL);
}


/**
 * @param char *str
 * @return char *
 */
char *trim_cpy(char *str)
{
    return trim_len_cpy(str, strlen(str), NULL);
}


/**
 * @param const char *cmd
 * @return std::string
 */
std::string shell_exec(const char *cmd)
{
    char    out[8096];
    char    *outret = out;
    size_t  outlen  = 0;
    FILE    *handle = popen(cmd, "r");

    if (handle == NULL) {
        int  ern = errno;
        char *cp = out;
        outlen   = 7;

        memcpy(cp, "Error: ", 7);
        cp += 7;
        strncpy(cp, strerror(ern), sizeof(out) - 8);
        outlen += strlen(cp);

        goto ret;
    }

    outlen = fread(out, sizeof(char), sizeof(out), handle);
    pclose(handle);
    outret = trim_len(out, outlen, &outlen);

ret:
    {
        std::string ret;
        ret.assign(outret, outlen);
        return ret;
    }
}
