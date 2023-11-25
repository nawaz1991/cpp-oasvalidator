/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/object_deserializer.hpp"
#include <gtest/gtest.h>

enum : size_t
{
    INPUT,
    START_CHAR,
    SKIP_NAME,
    KV_SEPARATOR,
    VK_SEPARATOR,
    IS_DEEP_OBJ,
    SHOULD_THROW
};

static const ObjKTMap KT_MAP = {{"\"boolTrue\"", PrimitiveType::BOOLEAN},
                                {"\"boolFalse\"", PrimitiveType::BOOLEAN},
                                {"\"int\"", PrimitiveType::INTEGER},
                                {"\"number\"", PrimitiveType::NUMBER},
                                {"\"string\"", PrimitiveType::STRING}};

static const std::string EXPECTED = R"({"boolTrue":true,"boolFalse":false,"int":123,"number":123.456,"string":"abc xyz"})";

class ObjectDeserializerTest: public ::testing::TestWithParam<std::tuple<std::string, char, bool, char, char, bool, bool>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        std::string input = std::get<INPUT>(params);
        char start_char = std::get<START_CHAR>(params);
        bool skip_name = std::get<SKIP_NAME>(params);
        char kv_separator = std::get<KV_SEPARATOR>(params);
        char vk_separator = std::get<VK_SEPARATOR>(params);
        bool is_deep_obj = std::get<IS_DEEP_OBJ>(params);
        bool should_throw = std::get<SHOULD_THROW>(params);

        deserializer_ = std::make_unique<ObjectDeserializer>("test", start_char, skip_name, kv_separator, vk_separator, is_deep_obj, KT_MAP);
        input_ = input;
        expect_throw_ = should_throw;
    }

    std::unique_ptr<ObjectDeserializer> deserializer_;
    std::string input_;
    bool expect_throw_;
};

TEST_P(ObjectDeserializerTest, Deserialize)
{
    if (expect_throw_) {
        EXPECT_THROW({ deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size()); }, DeserializationException);
    } else {
        std::string result = deserializer_->Deserialize(input_.c_str(), input_.c_str() + input_.size());
        EXPECT_EQ(result, EXPECTED);
    }
}

INSTANTIATE_TEST_SUITE_P(ObjectDeserializerTests, ObjectDeserializerTest,
                        ::testing::Values(std::make_tuple("boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", '\0', false, ',', ',', false, false),
                                          std::make_tuple("boolTrue=true,boolFalse=false,int=123,number=123.456,string=abc%20xyz", '\0', false, '=', ',', false, false),
                                          std::make_tuple(".boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", '.', false, ',', ',', false, false),
                                          std::make_tuple(".boolTrue=true,boolFalse=false,int=123,number=123.456,string=abc%20xyz", '.', false, '=', ',', false, false),
                                          std::make_tuple(";test=boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", ';', true, ',', ',', false, false),
                                          std::make_tuple(";boolTrue=true;boolFalse=false;int=123;number=123.456;string=abc%20xyz", ';', false, '=', ';', false, false),
                                          std::make_tuple("?boolTrue=true&boolFalse=false&int=123&number=123.456&string=abc%20xyz", '?', false, '=', '&', false, false),
                                          std::make_tuple("?test=boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", '?', true, ',', ',', false, false),
                                          std::make_tuple("?test[boolTrue]=true&test[boolFalse]=false&test[int]=123&test[number]=123.456&test[string]=abc%20xyz", '?', true, '=',
                                                          '&', true, false),
                                          std::make_tuple("boolTrue,true,boolFalse,false,int,str,number,123.456,string,abc%20xyz", '\0', false, ',', ',', false, true),
                                          std::make_tuple("boolTrue=true,boolFalse=false,int=123,number=123.456,string=abc%2xyz", '\0', false, '=', ',', false, true),
                                          std::make_tuple(".boolTrue,true,boolFalse,false,int,123,number,1.23.456,string,abc%20xyz", '.', false, ',', ',', false, true),
                                          std::make_tuple(".boolTrue;true,boolFalse=false,int=123,number=123.456,string=abc%20xyz", '.', false, '=', ',', false, true),
                                          std::make_tuple("test=boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", ';', true, ',', ',', false, true),
                                          std::make_tuple(";boolTrue,true;boolFalse=false;int=123;number=123.456;string=abc%20xyz", ';', false, '=', ';', false, true)));