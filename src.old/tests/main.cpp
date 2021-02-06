
#undef NDEBUG

#include <cstdio>
#include <cassert>
#include <cstring>

#include "curl.hpp"
#include "helpers.hpp"


int main()
{
    test_trim_implicit_len();
    test_trim_explicit_len();
    test_curl();
    return 0;
}
