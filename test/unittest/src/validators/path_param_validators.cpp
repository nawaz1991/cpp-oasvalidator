/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "validators/param_validators.hpp"
#include <gtest/gtest.h>
#include <tuple>

enum : size_t
{
    INPUT,
    STYLE,
    EXPLODE,
    TYPE,
    EXPECTED_ERROR
};

// Create a parameterized fixture class using the struct
class PathPrimitiveParam
    : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, std::string, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/" + std::get<INPUT>(params) + "/xyz";
        std::string style = std::get<STYLE>(params);
        bool explode = std::get<EXPLODE>(params);
        std::string type = std::get<TYPE>(params);
        expected_error_ = std::get<EXPECTED_ERROR>(params);

        std::string json = R"({
            "name": "param",
            "in": "path",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": ")" +
                           type + R"("
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<PathParamValidator>(doc_, keys_);
    }

    std::unique_ptr<PathParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/{param}/xyz", "get", "parameters", "param"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(PathPrimitiveParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/").size();
    const char* end = input_.c_str() + input_.size() - std::string("/xyz").size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_SUITE_P(
    PathParamValidatorTests, PathPrimitiveParam,
    ::testing::Values(std::make_tuple("true", "simple", false, "boolean", ValidationError::NONE),
                      std::make_tuple("true", "simple", true, "boolean", ValidationError::NONE),
                      std::make_tuple("false", "simple", false, "boolean", ValidationError::NONE),
                      std::make_tuple("false", "simple", true, "boolean", ValidationError::NONE),
                      std::make_tuple("123", "simple", false, "boolean", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple("123", "simple", false, "integer", ValidationError::NONE),
                      std::make_tuple("123", "simple", true, "integer", ValidationError::NONE),
                      std::make_tuple("123.0", "simple", false, "integer", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple("123.0", "simple", false, "number", ValidationError::NONE),
                      std::make_tuple("123.0", "simple", true, "number", ValidationError::NONE),
                      std::make_tuple("abc", "simple", false, "number", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple("abc%20xyz", "simple", false, "string", ValidationError::NONE),
                      std::make_tuple("abc%20xyz", "simple", true, "string", ValidationError::NONE),
                      std::make_tuple(".true", "label", false, "boolean", ValidationError::NONE),
                      std::make_tuple(".true", "label", true, "boolean", ValidationError::NONE),
                      std::make_tuple(".false", "label", false, "boolean", ValidationError::NONE),
                      std::make_tuple(".false", "label", true, "boolean", ValidationError::NONE),
                      std::make_tuple(".123", "label", false, "boolean", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(".123", "label", false, "integer", ValidationError::NONE),
                      std::make_tuple(".123", "label", true, "integer", ValidationError::NONE),
                      std::make_tuple(".123.0", "label", false, "integer", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(".abc%20xyz", "label", false, "string", ValidationError::NONE),
                      std::make_tuple(".abc%20xyz", "label", true, "string", ValidationError::NONE),
                      std::make_tuple(";param=true", "matrix", false, "boolean", ValidationError::NONE),
                      std::make_tuple(";param=true", "matrix", true, "boolean", ValidationError::NONE),
                      std::make_tuple(";param=false", "matrix", false, "boolean", ValidationError::NONE),
                      std::make_tuple(";param=false", "matrix", true, "boolean", ValidationError::NONE),
                      std::make_tuple(";param=123", "matrix", false, "boolean", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(";param=123", "matrix", false, "integer", ValidationError::NONE),
                      std::make_tuple(";param=123", "matrix", true, "integer", ValidationError::NONE),
                      std::make_tuple(";param=123.0", "matrix", false, "integer", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(";param=123.0", "matrix", false, "number", ValidationError::NONE),
                      std::make_tuple(";param=123.0", "matrix", true, "number", ValidationError::NONE),
                      std::make_tuple(";param=abc", "matrix", false, "number", ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(";param=abc%20xyz", "matrix", false, "string", ValidationError::NONE),
                      std::make_tuple(";param=abc%20xyz", "matrix", true, "string", ValidationError::NONE)));

class PathArrayParam
    : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, std::string, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/" + std::get<INPUT>(params) + "/xyz";
        std::string style = std::get<STYLE>(params);
        bool explode = std::get<EXPLODE>(params);
        std::string type = std::get<TYPE>(params);
        expected_error_ = std::get<EXPECTED_ERROR>(params);

        std::string json = R"({
            "name": "param",
            "in": "path",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": "array",
                "items": {
                    "type": ")" +
                           type + R"("
                }
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<PathParamValidator>(doc_, keys_);
    }

    std::unique_ptr<PathParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/{param}/xyz", "get", "parameters", "0"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(PathArrayParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/").size();
    const char* end = input_.c_str() + input_.size() - std::string("/xyz").size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_SUITE_P(
    PathParamValidatorTests, PathArrayParam,
    ::testing::Values(
        std::make_tuple("true,false", "simple", false, "boolean", ValidationError::NONE),
        std::make_tuple("true,false", "simple", true, "boolean", ValidationError::NONE),
        std::make_tuple("false,true", "simple", false, "boolean", ValidationError::NONE),
        std::make_tuple("false,true", "simple", true, "boolean", ValidationError::NONE),
        std::make_tuple("123,456", "simple", false, "boolean", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple("123,456", "simple", false, "integer", ValidationError::NONE),
        std::make_tuple("123,456", "simple", true, "integer", ValidationError::NONE),
        std::make_tuple("123.0,456.0", "simple", false, "integer", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple("123.0,456.0", "simple", false, "number", ValidationError::NONE),
        std::make_tuple("123.0,456.0", "simple", true, "number", ValidationError::NONE),
        std::make_tuple("abc,xyz", "simple", false, "number", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple("abc%20xyz,def%20ghi", "simple", false, "string", ValidationError::NONE),
        std::make_tuple("abc%20xyz,def%20ghi", "simple", true, "string", ValidationError::NONE),
        std::make_tuple(".true,false", "label", false, "boolean", ValidationError::NONE),
        std::make_tuple(".true.false", "label", true, "boolean", ValidationError::NONE),
        std::make_tuple(".false,true", "label", false, "boolean", ValidationError::NONE),
        std::make_tuple(".false.true", "label", true, "boolean", ValidationError::NONE),
        std::make_tuple(".123,456", "label", false, "boolean", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple(".123,456", "label", false, "integer", ValidationError::NONE),
        std::make_tuple(".123.456", "label", true, "integer", ValidationError::NONE),
        std::make_tuple(".123.0,456.0", "label", false, "integer", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple(".abc%20xyz,def%20ghi", "label", false, "string", ValidationError::NONE),
        std::make_tuple(".abc%20xyz.def%20ghi", "label", true, "string", ValidationError::NONE),
        std::make_tuple(";param=true,false", "matrix", false, "boolean", ValidationError::NONE),
        std::make_tuple(";param=true;param=false", "matrix", true, "boolean", ValidationError::NONE),
        std::make_tuple(";param=false,true", "matrix", false, "boolean", ValidationError::NONE),
        std::make_tuple(";param=false;param=true", "matrix", true, "boolean", ValidationError::NONE),
        std::make_tuple(";param=123,456", "matrix", false, "boolean", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple(";param=123;param=456", "matrix", true, "integer", ValidationError::NONE),
        std::make_tuple(";param=123.0,456.0", "matrix", false, "integer", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple(";param=123.0;param=456.0", "matrix", true, "number", ValidationError::NONE),
        std::make_tuple(";param=abc,xyz", "matrix", false, "number", ValidationError::INVALID_PATH_PARAM),
        std::make_tuple(";param=abc%20xyz,def%20ghi", "matrix", false, "string", ValidationError::NONE),
        std::make_tuple(";param=abc%20xyz;param=def%20ghi", "matrix", true, "string", ValidationError::NONE)));

class PathObjectParam: public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/" + std::get<0>(params) + "/xyz";
        std::string style = std::get<1>(params);
        bool explode = std::get<2>(params);
        expected_error_ = std::get<3>(params);

        std::string json = R"({
            "name": "param",
            "in": "path",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": "object",
                "properties": {
                    "boolTrue": {
                        "type": "boolean"
                    },
                    "boolFalse": {
                        "type": "boolean"
                    },
                    "int": {
                        "type": "integer",
                        "format": "int64"
                    },
                    "number": {
                        "type": "number",
                        "format": "double"
                    },
                    "string": {
                        "type": "string"
                    }
                }
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<PathParamValidator>(doc_, keys_);
    }

    std::unique_ptr<PathParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/{param}/xyz", "get", "parameters", "0"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(PathObjectParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/").size();
    const char* end = input_.c_str() + input_.size() - std::string("/xyz").size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_SUITE_P(
    PathParamValidatorTests, PathObjectParam,
    ::testing::Values(std::make_tuple("boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", "simple",
                                      false, ValidationError::NONE),
                      std::make_tuple("boolTrue=true,boolFalse=false,int=123,number=123.456,string=abc%20xyz", "simple",
                                      true, ValidationError::NONE),
                      std::make_tuple(".boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", "label",
                                      false, ValidationError::NONE),
                      std::make_tuple(";param=boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz",
                                      "matrix", false, ValidationError::NONE),
                      std::make_tuple(";boolTrue=true;boolFalse=false;int=123;number=123.456;string=abc%20xyz",
                                      "matrix", true, ValidationError::NONE),
                      std::make_tuple("boolTrue,ture,boolFalse,false,int,123,number,123.456,string,abc%20xyz", "simple",
                                      false, ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple("boolTrue=true,boolFalse=false,int=1.23,number=123.456,string=abc%20xyz",
                                      "simple", false, ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple("boolTrue=true,boolFalse=false,int=123,number=123.456,string=abc%2xyz", "simple",
                                      false, ValidationError::INVALID_PATH_PARAM),
                      std::make_tuple(".boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", "label",
                                      true, ValidationError::INVALID_PATH_PARAM)));