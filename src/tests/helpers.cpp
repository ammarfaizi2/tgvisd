
#undef NDEBUG

#include <cstdio>
#include <cassert>
#include <cstring>
#include <TeaBot/helpers.hpp>

inline static void test_trim_implicit_len();
inline static void test_trim_explicit_len();

int main()
{
    test_trim_implicit_len();
    test_trim_explicit_len();
    return 0;
}


/**
 * @return void
 */
inline static void test_trim_implicit_len()
{
    {
        char input[]  = "\r\n\r\n  Hello World  \r\n\r\n";
        char expect[] = "\r\n\r\n  Hello World\0 \r\n\r\n";
        assert(trim(input) == (input + 6));
        assert(memcmp(input, expect, sizeof(expect)) == 0);
    }


    {
        char input[]  = "  Hello World  \r\n\r\n";
        char expect[] = "Hello World\0d  \r\n\r\n";
        assert(trim_cpy(input) == input);
        assert(memcmp(input, expect, sizeof(expect)) == 0);
    }

    {
        char input[]  = " ";
        char expect[] = " ";
        assert(trim(input) == (input + 1));
        assert(memcmp(input, expect, sizeof(expect)) == 0);
    }

    {
        char input[]  = "     ";
        char expect[] = "\0    ";
        assert(trim_cpy(input) == input);
        assert(memcmp(input, expect, sizeof(expect)) == 0);
    }
}


/**
 * @return void
 */
inline static void test_trim_explicit_len()
{
    {
        char input[]   = "\r\n\r\n  Hello World  \r\n\r\n";
        char expect[]  = "\r\n\r\n  Hello World\0 \r\n\r\n";
        size_t res_len = 0;
        assert(trim_len(input, sizeof(input)-1, &res_len) == (input + 6));
        assert(memcmp(input, expect, sizeof(expect)) == 0);
        assert(res_len == 11);
    }

    {
        char input[]   = "  Hello World  \r\n\r\n";
        char expect[]  = "Hello World\0d  \r\n\r\n";
        size_t res_len = 0;
        assert(trim_len_cpy(input, sizeof(input)-1, &res_len) == input);
        assert(memcmp(input, expect, sizeof(expect)) == 0);
        assert(res_len == 11);
    }

    {
        char input[]   = " ";
        char expect[]  = " ";
        size_t res_len = -1;
        assert(trim_len(input, sizeof(input)-1, &res_len) == (input + 1));
        assert(memcmp(input, expect, sizeof(expect)) == 0);
        assert(res_len == 0);
    }

    {
        char input[]  = "     ";
        char expect[] = "\0    ";
        size_t res_len = -1;
        assert(trim_len_cpy(input, sizeof(input)-1, &res_len) == input);
        assert(memcmp(input, expect, sizeof(expect)) == 0);
        assert(res_len == 0);
    }
}