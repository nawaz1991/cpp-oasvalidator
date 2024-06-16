/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "deserializers/primitive_deserializer.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <tuple>

enum : size_t
{
    INPUT,
    TYPE,
    EXPECTED,
    START_CHAR,
    SKIP_NAME,
    SHOULD_THROW
};

class PrimitiveDeserializerTest
    : public ::testing::TestWithParam<std::tuple<std::string, PrimitiveType, std::string, char, bool, bool>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        std::string input = std::get<INPUT>(params);
        PrimitiveType type = std::get<TYPE>(params);
        std::string expected = std::get<EXPECTED>(params);
        char start_char = std::get<START_CHAR>(params);
        bool skip_name = std::get<SKIP_NAME>(params);
        bool should_throw = std::get<SHOULD_THROW>(params);

        deserializer_ = std::make_unique<PrimitiveDeserializer>("test", start_char, skip_name, type);
        input_ = input;
        expected_ = expected;
        expect_throw_ = should_throw;
    }

    std::unique_ptr<PrimitiveDeserializer> deserializer_;
    std::string input_;
    std::string expected_;
    bool expect_throw_;
};

TEST_P(PrimitiveDeserializerTest, Deserialize)
{
    if (expect_throw_) {
        EXPECT_THROW({ deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size()); },
                     DeserializationException);
    } else {
        std::string result = deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size());
        EXPECT_EQ(result, expected_);
    }
}

INSTANTIATE_TEST_SUITE_P(
    PrimitiveDeserializerTests, PrimitiveDeserializerTest,
    ::testing::Values(std::make_tuple("true", PrimitiveType::BOOLEAN, "true", '\0', false, false),
                      std::make_tuple("123", PrimitiveType::INTEGER, "123", '\0', false, false),
                      std::make_tuple("123.456", PrimitiveType::NUMBER, "123.456", '\0', false, false),
                      std::make_tuple("abc", PrimitiveType::STRING, "\"abc\"", '\0', false, false),
                      std::make_tuple("test=true", PrimitiveType::BOOLEAN, "true", '\0', true, false),
                      std::make_tuple("test=123", PrimitiveType::INTEGER, "123", '\0', true, false),
                      std::make_tuple("test=123.456", PrimitiveType::NUMBER, "123.456", '\0', true, false),
                      std::make_tuple("test=abc%20xyz", PrimitiveType::STRING, "\"abc xyz\"", '\0', true, false),
                      std::make_tuple(".true", PrimitiveType::BOOLEAN, "true", '.', false, false),
                      std::make_tuple(".123", PrimitiveType::INTEGER, "123", '.', false, false),
                      std::make_tuple(".123.456", PrimitiveType::NUMBER, "123.456", '.', false, false),
                      std::make_tuple(".abc", PrimitiveType::STRING, "\"abc\"", '.', false, false),
                      std::make_tuple(";test=true", PrimitiveType::BOOLEAN, "true", ';', true, false),
                      std::make_tuple(";test=123", PrimitiveType::INTEGER, "123", ';', true, false),
                      std::make_tuple(";test=123.456", PrimitiveType::NUMBER, "123.456", ';', true, false),
                      std::make_tuple(";test=abc", PrimitiveType::STRING, "\"abc\"", ';', true, false),
                      std::make_tuple("invalid", PrimitiveType::BOOLEAN, "invalid", '\0', false, true),
                      std::make_tuple("invalid", PrimitiveType::INTEGER, "invalid", '\0', false, true),
                      std::make_tuple("invalid", PrimitiveType::NUMBER, "invalid", '\0', false, true),
                      std::make_tuple("inva%lid", PrimitiveType::STRING, "invalid", '\0', false, true)));