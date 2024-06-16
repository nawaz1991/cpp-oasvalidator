/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "validators/method_validator.hpp"
#include <gtest/gtest.h>
#include <rapidjson/document.h>
TEST(MethodValidatorTest, Positive)
{
    MethodValidator validator;
    std::string err_msg;

    EXPECT_EQ(validator.Validate("GET", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("POST", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("PUT", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("DELETE", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("HEAD", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("OPTIONS", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("TRACE", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("CONNECT", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("PATCH", err_msg), ValidationError::NONE);

    EXPECT_EQ(validator.Validate("get", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("post", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("put", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("delete", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("head", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("options", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("trace", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("connect", err_msg), ValidationError::NONE);
    EXPECT_EQ(validator.Validate("patch", err_msg), ValidationError::NONE);
}

TEST(MethodValidatorTest, Negative)
{
    MethodValidator validator;
    std::string err_msg;
    rapidjson::Document doc;

    EXPECT_EQ(validator.Validate("GETT", err_msg), ValidationError::INVALID_METHOD);
    doc.Parse(err_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(validator.Validate("POSTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("PUTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("DELETET", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("HEADT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("OPTIONST", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("TRACET", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("CONNECTT", err_msg), ValidationError::INVALID_METHOD);
    EXPECT_EQ(validator.Validate("PATCHT", err_msg), ValidationError::INVALID_METHOD);
}