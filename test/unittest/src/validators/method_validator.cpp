/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "validators/method_validator.hpp"
#include <gtest/gtest.h>
#include <rapidjson/document.h>
TEST(MethodValidatorTest, Positive)
{
    MethodValidator validator;
    std::string err_msg;

    EXPECT_EQ(validator.ValidateJson("GET", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("POST", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("PUT", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("DELETE", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("HEAD", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("OPTIONS", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("TRACE", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("CONNECT", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("PATCH", err_msg), ValidationError::NONE);

    EXPECT_EQ(validator.ValidateJson("get", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("post", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("put", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("delete", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("head", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("options", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("trace", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("connect", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.ValidateJson("patch", err_msg), ValidationError::NONE);
}

TEST(MethodValidatorTest, Negative)
{
    MethodValidator validator;
    std::string err_msg;
    rapidjson::Document doc;

    EXPECT_EQ(validator.ValidateJson("GETT", err_msg), ValidationError::INVALID_METHOD);
    doc.Parse(err_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(validator.ValidateJson("POSTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("PUTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("DELETET", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("HEADT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("OPTIONST", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("TRACET", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("CONNECTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.ValidateJson("PATCHT", err_msg), ValidationError::INVALID_METHOD);
}