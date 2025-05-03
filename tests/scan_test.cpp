#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <print>
#include <string_view>

#include "scan.hpp"

TEST(ScanTest, SingleStringTest) {
    auto result = stdx::scan<std::string>("number", "{}");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), std::string_view( "number" ) );
}