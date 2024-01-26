/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "oas_validator.hpp"
#include "utils/common.hpp"
#include <gtest/gtest.h>

TEST(OASValidatorImpTest, ValidateRoute)
{
    EXPECT_THROW({ OASValidator validator("invalid_path"); }, ValidatorInitExc);
}

class OASValidatorTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        validator_ = std::make_unique<OASValidator>(SPEC_PATH);
    }
    std::unique_ptr<OASValidator> validator_;
};

TEST_F(OASValidatorTest, ValidateRoute)
{
    std::string err_msg;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_simple_true/123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_simple_true/123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_label_true/.123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_label_true/123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_matrix_true/;param=123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/integer_matrix_true/;123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_simple_true/abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_simple_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_label_true/.abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_label_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_matrix_true/;param=abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRoute("GET", "/test/string_matrix_true/;param=abc%2xyz", err_msg));
}

TEST_F(OASValidatorTest, ValidatePathParam)
{
    std::string err_msg;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidatePathParam("GET", "/test/integer_simple_true/123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/integer_simple_true/123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidatePathParam("GET", "/test/integer_label_true/.123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/integer_label_true/123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidatePathParam("GET", "/test/integer_matrix_true/;param=123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/integer_matrix_true/;123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidatePathParam("GET", "/test/string_simple_true/abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/string_simple_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/string_label_true/.abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/string_label_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidatePathParam("GET", "/test/string_matrix_true/;param=abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidatePathParam("GET", "/test/string_matrix_true/;param=abc%2xyz", err_msg));
}

TEST_F(OASValidatorTest, ValidateQueryParam)
{
    std::string err_msg;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateQueryParam("GET", "/test/query_integer_form_true?param=123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_QUERY_PARAM, validator_->ValidateQueryParam("GET", "/test/query_integer_form_true?param=123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateQueryParam("GET", "/test/query_two_integer_form_mixed?param1=123&param2=456", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateQueryParam("GET", "/test/query_optional", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateQueryParam("GET", "/test/query_optional?", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateQueryParam("GET", "/test/query_optional?param=10", err_msg));
}

TEST_F(OASValidatorTest, ValidateHeaders)
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["intHeader"] = "123";
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateHeaders("GET", "/test/header_single1", headers, err_msg));
    headers["intHeader"] = "123str";
    EXPECT_EQ(ValidationError::INVALID_HEADER_PARAM, validator_->ValidateHeaders("GET", "/test/header_single1", headers, err_msg));
    headers.clear();
    headers["objectHeader"] = "field1,123,field2,abc";
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateHeaders("GET", "/test/header_single5", headers, err_msg));
    headers["objectHeader"] = "field1,xyz,field2,abc";
    EXPECT_EQ(ValidationError::INVALID_HEADER_PARAM, validator_->ValidateHeaders("GET", "/test/header_single5", headers, err_msg));
    headers.clear();
    headers["objectHeader"] = "field1=123,field2=abc";
    headers["stringHeader"] = "abc";
    headers["intHeader"] = "123";
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateHeaders("GET", "/test/header_triple5", headers, err_msg));
    headers["objectHeader"] = "field1,123,field2,abc";
    EXPECT_EQ(ValidationError::INVALID_HEADER_PARAM, validator_->ValidateHeaders("GET", "/test/header_triple5", headers, err_msg));
}

TEST_F(OASValidatorTest, ValidateBody)
{
    std::string err_msg;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateBody("POST", "/test/body_scenario1", "123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_BODY, validator_->ValidateBody("POST", "/test/body_scenario1", "123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateBody("POST", "/test/body_scenario20", "{\"level1\":{\"level2\":{\"level3\":\"abc\"}}}", err_msg));
    EXPECT_EQ(ValidationError::INVALID_BODY, validator_->ValidateBody("POST", "/test/body_scenario20", "{\"level1\":{\"level2\":{\"level3\":123}}}", err_msg));
}

TEST_F(OASValidatorTest, ValidateRequst)
{
    std::string err_msg;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_simple_true/123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/integer_simple_true/123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_label_true/.123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/integer_label_true/123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_matrix_true/;param=123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/integer_matrix_true/;123", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/string_simple_true/abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/string_simple_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/string_label_true/.abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/string_label_true/abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/string_matrix_true/;param=abc%20xyz", err_msg));
    EXPECT_EQ(ValidationError::INVALID_PATH_PARAM, validator_->ValidateRequest("GET", "/test/string_matrix_true/;param=abc%2xyz", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/query_integer_form_true?param=123", err_msg));
    EXPECT_EQ(ValidationError::INVALID_QUERY_PARAM, validator_->ValidateRequest("GET", "/test/query_integer_form_true?param=123str", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/query_two_integer_form_mixed?param1=123&param2=456", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/header_single1", err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_simple_true/123", "", err_msg));
    std::unordered_map<std::string, std::string> dummy;
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_simple_true/123", "", dummy, err_msg));
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("GET", "/test/integer_simple_true/123", dummy, err_msg));
    EXPECT_EQ(ValidationError::INVALID_METHOD, validator_->ValidateRequest("GET1", "/test/integer_simple_true/123", dummy, err_msg));
    EXPECT_EQ(ValidationError::INVALID_ROUTE, validator_->ValidateRequest("GET", "/test/invalid", dummy, err_msg));
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    headers["param12"] = "field1=0&field2=string";
    EXPECT_EQ(ValidationError::NONE, validator_->ValidateRequest("POST",
                                                                 "/test/all/123/abc/str1,str2/"
                                                                 "field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&"
                                                                 "param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
                                                                 "{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":"
                                                                 "\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":\"abc\",\"field7\":[123,456],\"field8\":[123,456],\"field9\":"
                                                                 "\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}",
                                                                 headers, err_msg));
    EXPECT_EQ(ValidationError::INVALID_BODY,
              validator_->ValidateRequest("POST",
                                          "/test/all/123/abc/str1,str2/"
                                          "field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&"
                                          "param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
                                          "{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":"
                                          "\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":true,\"field7\":[123,456],\"field8\":[123,456],\"field9\":"
                                          "\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}",
                                          headers, err_msg));
    EXPECT_EQ(ValidationError::NONE,
              validator_->ValidateRequest("GET",
                                          "/test/content/123/abc/%5B%22string1%22%2C%22string2%22%5D/"
                                          "%7B%0A%20%20%22field1%22%3A%200%2C%0A%20%20%22field2%22%3A%20%22string%22%0A%7D?param4=%5B%22str1%22%2C%22str2%22%5D&param5=%7B%0A%20%"
                                          "20%22field1%22%3A%200%2C%0A%20%20%22field2%22%3A%20%22string%22%0A%7D&param6=%7B%0A%20%20%22field1%22%3A%200%2C%0A%20%20%22field2%22%3A%"
                                          "20%22string%22%0A%7D&param7=%7B%0A%20%20%22field1%22%3A%200%2C%0A%20%20%22field2%22%3A%20%22string%22%0A%7D",
                                          err_msg));
}