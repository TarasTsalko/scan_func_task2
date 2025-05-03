#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <print>
#include <string_view>
#include <typeinfo>

#include "scan.hpp"

TEST(ScanTest, SingleStringTest) {
    auto result = stdx::scan<std::string>("number", "{}");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), std::string_view( "number" ) );
}

TEST(ScanTest, IntFloatInStringTest) {
    auto result = stdx::scan<int8_t, float>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), 42 );
    ASSERT_FLOAT_EQ( values.value<1>(), 3.14 );
    ASSERT_TRUE( typeid( decltype( values.value<0>() ) ).name() == typeid(int8_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<1>() ) ).name() == typeid(float).name() );
}

TEST(ScanTest, IntNegativeFloatInStringTest) {
    auto result = stdx::scan<int8_t, float>("I want to sum 42 and -3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), 42 );
    ASSERT_FLOAT_EQ( values.value<1>(), -3.14 );
    ASSERT_TRUE( typeid( decltype( values.value<0>() ) ).name() == typeid(int8_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<1>() ) ).name() == typeid(float).name() );
}

TEST(ScanTest, IntDoubleInStringTest) {
    auto result = stdx::scan<int8_t, double>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), 42 );
    ASSERT_FLOAT_EQ( values.value<1>(), 3.14 );
}

TEST(ScanTest, IntNegativeDoubleInStringTest) {
    auto result = stdx::scan<int8_t, double>("I want to sum 42 and -3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(result.has_value()) << result.error();
    const auto values = result.value();
    ASSERT_EQ( values.value<0>(), 42 );
    ASSERT_FLOAT_EQ( values.value<1>(), -3.14 );
}

TEST(ScanTest, DiferentsIntsInString) {
    auto result = stdx::scan<int8_t, int16_t, int32_t>( "int8_t val 8, int16_t val 16, int32_t val 32", "int8_t val {}, int16_t val {}, int32_t val {}");
    ASSERT_TRUE(result.has_value()) << result.error();
    auto values = result.value();

    ASSERT_EQ( values.value<0>(), 8 );
    ASSERT_EQ( values.value<1>(), 16 );
    ASSERT_EQ( values.value<2>(), 32 );
    ASSERT_TRUE( typeid( decltype( values.value<0>() ) ).name() == typeid(int8_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<1>() ) ).name() == typeid(int16_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<2>() ) ).name() == typeid(int32_t).name() );
}

TEST(ScanTest, DiferentsNegativeIntsInString) {
    auto result = stdx::scan<int8_t, int16_t, int32_t>( "int8_t val -8, int16_t val -16, int32_t val -32", "int8_t val {}, int16_t val {}, int32_t val {}");
    ASSERT_TRUE(result.has_value()) << result.error();
    auto values = result.value();

    ASSERT_EQ( values.value<0>(), -8 );
    ASSERT_EQ( values.value<1>(), -16 );
    ASSERT_EQ( values.value<2>(), -32 );
    ASSERT_TRUE( typeid( decltype( values.value<0>() ) ).name() == typeid(int8_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<1>() ) ).name() == typeid(int16_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<2>() ) ).name() == typeid(int32_t).name() );
}

TEST(ScanTest, DiferentsUIntsInString) {
    auto result = stdx::scan<uint8_t, uint16_t, uint32_t>( "uint8_t val 8, uint16_t val 16, uint32_t val 32", "uint8_t val {}, uint16_t val {}, uint32_t val {}");
    ASSERT_TRUE(result.has_value()) << result.error();
    auto values = result.value();

    ASSERT_EQ( values.value<0>(), 8u );
    ASSERT_EQ( values.value<1>(), 16u );
    ASSERT_EQ( values.value<2>(), 32u );
    ASSERT_TRUE( typeid( decltype( values.value<0>() ) ).name() == typeid(uint8_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<1>() ) ).name() == typeid(uint16_t).name() );
    ASSERT_TRUE( typeid( decltype( values.value<2>() ) ).name() == typeid(uint32_t).name() );
}