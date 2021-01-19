
#undef NDEBUG

#include <cstdio>
#include <cassert>
#include <cstring>
#include <TeaBot/helpers.hpp>

inline static void test_trim();

int main()
{
    test_trim();
    return 0;
}


/**
 * @return void
 */
inline static void test_trim()
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
}
