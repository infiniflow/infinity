#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <cstddef>
#include <iostream> // needed in this test, see https://github.com/onqtam/doctest/issues/126#issuecomment-381746547
#include <string>

namespace {

std::string to_s(uint64_t n) {
    std::string str;
    do {
        str += static_cast<char>('0' + static_cast<char>(n % 10));
        n /= 10;
    } while (n != 0);
    std::reverse(str.begin(), str.end());
    return str;
}

} // namespace

TEST_CASE("to_s") {
    REQUIRE(to_s(UINT64_C(123)) == "123");
    REQUIRE(to_s(UINT64_C(0)) == "0");
    REQUIRE(to_s(UINT64_C(10)) == "10");
    REQUIRE(to_s(UINT64_C(123456789)) == "123456789");
    REQUIRE(to_s(UINT64_C(9876543210)) == "9876543210");
    REQUIRE(to_s(UINT64_C(18446744073709551615)) == "18446744073709551615");
}
