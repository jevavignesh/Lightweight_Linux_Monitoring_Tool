#include "lmon/strings.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace {

using namespace std::string_view_literals;

TEST(Strings, TrimRemovesSurroundingWhitespace) {
    EXPECT_EQ(lmon::trim("  hello world \n"), "hello world"sv);
    EXPECT_EQ(lmon::trim("\t\n "), ""sv);
    EXPECT_EQ(lmon::trim(""), ""sv);
}

TEST(Strings, SplitWhitespaceSkipsEmptyFields) {
    const auto fields = lmon::split_whitespace("cpu  4705 356\t584   3699\n");
    const std::vector<std::string_view> expected{"cpu", "4705", "356", "584", "3699"};
    EXPECT_EQ(fields, expected);
}

TEST(Strings, SplitWhitespaceOfBlankIsEmpty) {
    EXPECT_TRUE(lmon::split_whitespace("   ").empty());
}

TEST(Strings, ParseNumberIntegers) {
    EXPECT_EQ(lmon::parse_number<std::uint64_t>(" 16318400 "), 16318400U);
    EXPECT_EQ(lmon::parse_number<int>("-42"), -42);
    EXPECT_FALSE(lmon::parse_number<int>("12kB").has_value());
    EXPECT_FALSE(lmon::parse_number<int>("").has_value());
    EXPECT_FALSE(lmon::parse_number<std::uint8_t>("300").has_value());
}

TEST(Strings, ParseNumberFloatingPoint) {
    const auto value = lmon::parse_number<double>("12345.67");
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 12345.67);
}

} // namespace
