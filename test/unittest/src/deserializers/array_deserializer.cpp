/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/array_deserializer.hpp"
#include <gtest/gtest.h>

enum : size_t
{
    INPUT,
    START_CHAR,
    SKIP_NAME,
    ITEMS_TYPE,
    SEPARATOR,
    HAS_RUNNING_NAME,
    HAS_20_SEPARATOR,
    EXPECTED,
    SHOULD_THROW
};

class ArrayDeserializerTest: public ::testing::TestWithParam<std::tuple<std::string, char, bool, PrimitiveType, char, bool, bool, std::string, bool>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        std::string input = std::get<INPUT>(params);
        char start_char = std::get<START_CHAR>(params);
        bool skip_name = std::get<SKIP_NAME>(params);
        PrimitiveType items_type = std::get<ITEMS_TYPE>(params);
        char separator = std::get<SEPARATOR>(params);
        bool has_running_name = std::get<HAS_RUNNING_NAME>(params);
        bool has_20_separator = std::get<HAS_20_SEPARATOR>(params);
        std::string expected = std::get<EXPECTED>(params);
        bool should_throw = std::get<SHOULD_THROW>(params);

        deserializer_ = std::make_unique<ArrayDeserializer>("test", start_char, skip_name, items_type, separator, has_running_name, has_20_separator);
        input_ = input;
        expected_ = expected;
        expect_throw_ = should_throw;
    }

    std::unique_ptr<ArrayDeserializer> deserializer_;
    std::string input_;
    std::string expected_;
    bool expect_throw_;
};
TEST_P(ArrayDeserializerTest, Deserialize)
{
    if (expect_throw_) {
        EXPECT_THROW({ deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size()); }, DeserializationException);
    } else {
        std::string result = deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size());
        EXPECT_EQ(result, expected_);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ArrayDeserializerTests, ArrayDeserializerTest,
    ::testing::Values(std::make_tuple("true,false", '\0', false, PrimitiveType::BOOLEAN, ',', false, false, "[true,false]", false),
                      std::make_tuple("1,2,3", '\0', false, PrimitiveType::INTEGER, ',', false, false, "[1,2,3]", false),
                      std::make_tuple("1.1,2.2,3.3", '\0', false, PrimitiveType::NUMBER, ',', false, false, "[1.1,2.2,3.3]", false),
                      std::make_tuple("abc,def,ghi", '\0', false, PrimitiveType::STRING, ',', false, false, "[\"abc\",\"def\",\"ghi\"]", false),
                      std::make_tuple("test=true,false", '\0', true, PrimitiveType::BOOLEAN, ',', false, false, "[true,false]", false),
                      std::make_tuple("test=1,2,3", '\0', true, PrimitiveType::INTEGER, ',', false, false, "[1,2,3]", false),
                      std::make_tuple("test=1.1,2.2,3.3", '\0', true, PrimitiveType::NUMBER, ',', false, false, "[1.1,2.2,3.3]", false),
                      std::make_tuple("test=abc,def,ghi", '\0', true, PrimitiveType::STRING, ',', false, false, "[\"abc\",\"def\",\"ghi\"]", false),
                      std::make_tuple(".true,false", '.', false, PrimitiveType::BOOLEAN, ',', false, false, "[true,false]", false),
                      std::make_tuple(".1,2,3", '.', false, PrimitiveType::INTEGER, ',', false, false, "[1,2,3]", false),
                      std::make_tuple(".1.1,2.2,3.3", '.', false, PrimitiveType::NUMBER, ',', false, false, "[1.1,2.2,3.3]", false),
                      std::make_tuple(".abc,def,ghi", '.', false, PrimitiveType::STRING, ',', false, false, "[\"abc\",\"def\",\"ghi\"]", false),
                      std::make_tuple(".true.false", '.', false, PrimitiveType::BOOLEAN, '.', false, false, "[true,false]", false),
                      std::make_tuple(".1.2.3", '.', false, PrimitiveType::INTEGER, '.', false, false, "[1,2,3]", false),
                      // std::make_tuple(".1.1.2.2.3.3", '.', false, PrimitiveType::NUMBER, '.', false, false, "[1.1,2.2,3.3]", false), // NOT a valid serialization
                      std::make_tuple(".abc.def.ghi", '.', false, PrimitiveType::STRING, '.', false, false, "[\"abc\",\"def\",\"ghi\"]", false),
                      std::make_tuple(";test=true,false", ';', true, PrimitiveType::BOOLEAN, ',', false, false, "[true,false]", false),
                      std::make_tuple(";test=1,2,3", ';', true, PrimitiveType::INTEGER, ',', false, false, "[1,2,3]", false),
                      std::make_tuple(";test=1.1,2.2,3.3", ';', true, PrimitiveType::NUMBER, ',', false, false, "[1.1,2.2,3.3]", false),
                      std::make_tuple(";test=abc,def,ghi", ';', true, PrimitiveType::STRING, ',', false, false, "[\"abc\",\"def\",\"ghi\"]", false),
                      std::make_tuple(";test=true;test=false", ';', true, PrimitiveType::BOOLEAN, ';', true, false, "[true,false]", false),
                      std::make_tuple(";test=1;test=2", ';', true, PrimitiveType::INTEGER, ';', true, false, "[1,2]", false),
                      std::make_tuple(";test=1.1;test=2.2", ';', true, PrimitiveType::NUMBER, ';', true, false, "[1.1,2.2]", false),
                      std::make_tuple(";test=abc;test=def", ';', true, PrimitiveType::STRING, ';', true, false, "[\"abc\",\"def\"]", false),
                      std::make_tuple("?test=true&test=false", '?', true, PrimitiveType::BOOLEAN, '&', true, false, "[true,false]", false),
                      std::make_tuple("?test=1&test=2", '?', true, PrimitiveType::INTEGER, '&', true, false, "[1,2]", false),
                      std::make_tuple("?test=1.1&test=2.2", '?', true, PrimitiveType::NUMBER, '&', true, false, "[1.1,2.2]", false),
                      std::make_tuple("?test=abc&test=def", '?', true, PrimitiveType::STRING, '&', true, false, "[\"abc\",\"def\"]", false),
                      std::make_tuple("?test=true%20false", '?', true, PrimitiveType::BOOLEAN, '%', false, true, "[true,false]", false),
                      std::make_tuple("?test=1%202", '?', true, PrimitiveType::INTEGER, '%', false, true, "[1,2]", false),
                      std::make_tuple("?test=1.1%202.2", '?', true, PrimitiveType::NUMBER, '%', false, true, "[1.1,2.2]", false),
                      std::make_tuple("?test=abc%20def", '?', true, PrimitiveType::STRING, '%', false, true, "[\"abc\",\"def\"]", false),
                      std::make_tuple("true-false", '\0', false, PrimitiveType::BOOLEAN, ',', false, false, "[true,false]", true),
                      std::make_tuple("abc-true-5.3", '\0', false, PrimitiveType::INTEGER, ',', false, false, "[1,2,3]", true),
                      std::make_tuple("abc-true-5.3", '\0', false, PrimitiveType::NUMBER, ',', false, false, "[1.1,2.2,3.3]", true),
                      std::make_tuple("abc%true-5.3", '\0', false, PrimitiveType::STRING, ',', false, false, "[\"abc\",\"def\",\"ghi\"]", true)));
