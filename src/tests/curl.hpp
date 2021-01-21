
#include <TeaBot/TeaCurl.hpp>

inline static void test_curl()
{
    char   *ret;
    size_t len;
    TeaBot::TeaCurl cr("https://www.kernel.org/releases.json");
    cr.execute();

    ret = cr.get_output();
    len = cr.get_outlen();

    printf("Curl bytes: %lu\n", len);
    free(ret);
}
