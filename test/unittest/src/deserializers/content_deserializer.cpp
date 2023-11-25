/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/content_deserializer.hpp"
#include <gtest/gtest.h>

enum : size_t
{
    INPUT,
    START_CHAR,
    SKIP_NAME,
    SHOULD_THROW
};

static const std::string EXPECTED = R"({"boolTrue":true,"boolFalse":false,"int":123,"number":123.456,"string":"abc xyz"})";

class ContentDeserializerTest: public ::testing::TestWithParam<std::tuple<std::string, char, bool, bool>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        std::string input = std::get<INPUT>(params);
        char start_char = std::get<START_CHAR>(params);
        bool skip_name = std::get<SKIP_NAME>(params);
        bool should_throw = std::get<SHOULD_THROW>(params);

        deserializer_ = std::make_unique<ContentDeserializer>("test", start_char, skip_name);
        input_ = input;
        expect_throw_ = should_throw;
    }

    std::unique_ptr<ContentDeserializer> deserializer_;
    std::string input_;
    bool expect_throw_;
};

TEST_P(ContentDeserializerTest, Deserialize)
{
    if (expect_throw_) {
        EXPECT_THROW({ deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size()); }, DeserializationException);
    } else {
        std::string result = deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size());
        EXPECT_EQ(result, EXPECTED);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ContentDeserializerTests, ContentDeserializerTest,
    ::testing::Values(std::make_tuple("%7B%22boolTrue%22%3Atrue%2C%22boolFalse%22%3Afalse%2C%22int%22%3A123%2C%22number%22%3A123.456%2C%22string%22%3A%22abc%20xyz%22%7D", '\0',
                                      false, false),
                      std::make_tuple("test=%7B%22boolTrue%22%3Atrue%2C%22boolFalse%22%3Afalse%2C%22int%22%3A123%2C%"
                                      "22number%22%3A123.456%2C%22string%22%3A%22abc%20xyz%22%7D",
                                      '\0', true, false),
                      std::make_tuple("?test=%7B%22boolTrue%22%3Atrue%2C%22boolFalse%22%3Afalse%2C%22int%22%3A123%2C%"
                                      "22number%22%3A123.456%2C%22string%22%3A%22abc%20xyz%22%7D",
                                      '?', true, false),
                      std::make_tuple("test%7B%22boolTrue%22%3Atrue%2C%22boolFalse%22%3Afalse%2C%22int%22%3A123%2C%"
                                      "22number%22%3A123.456%2C%22string%22%3A%22abc%"
                                      "20xyz%22%7D&test2=123",
                                      '\0', true, true)));