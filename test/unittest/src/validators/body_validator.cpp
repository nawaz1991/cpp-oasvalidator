/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "validators/body_validator.hpp"
#include <gtest/gtest.h>

class TestBodyValidator: public ::testing::Test
{
protected:
    void SetUp() override
    {
        schema_doc_.Parse(R"({
                "$schema": "http://json-schema.org/draft-05/schema#",
                "title": "UserProfile",
                "description": "A user profile with various attributes",
                "type": "object",
                "properties": {
                  "userId": {
                    "description": "The unique identifier for a user",
                    "type": "integer",
                    "minimum": 1
                  },
                  "username": {
                    "description": "The user's username",
                    "type": "string",
                    "minLength": 3,
                    "maxLength": 20,
                    "pattern": "^[a-zA-Z0-9_]+$"
                  },
                  "email": {
                    "type": "string",
                    "format": "email"
                  },
                  "age": {
                    "description": "Age in years which must be a positive integer",
                    "type": "integer",
                    "minimum": 18,
                    "maximum": 100
                  },
                  "phoneNumber": {
                    "type": ["string", "null"],
                    "pattern": "^\\+?[1-9]\\d{1,14}$"
                  },
                  "address": {
                    "type": "object",
                    "properties": {
                      "street": {"type": "string"},
                      "city": {"type": "string"},
                      "state": {"type": "string"},
                      "zipCode": {
                        "type": "string",
                        "pattern": "^\\d{5}(-\\d{4})?$"
                      }
                    },
                    "required": ["street", "city", "state", "zipCode"],
                    "additionalProperties": false
                  },
                  "subscriptionType": {
                    "type": "string",
                    "enum": ["basic", "premium", "gold"]
                  },
                  "preferences": {
                    "type": "object",
                    "properties": {
                      "newsletter": {
                        "type": "boolean",
                        "default": false
                      },
                      "themes": {
                        "type": "array",
                        "items": {
                          "type": "string",
                          "enum": ["light", "dark", "colorful"]
                        },
                        "minItems": 1,
                        "uniqueItems": true
                      }
                    },
                    "additionalProperties": false
                  },
                  "createdAt": {
                    "type": "string",
                    "format": "date-time"
                  }
                },
                "required": ["userId", "username", "email", "createdAt"],
                "additionalProperties": false,
                "dependencies": {
                  "phoneNumber": ["address"]
                },
                "allOf": [
                  {
                    "if": {
                      "properties": {"subscriptionType": {"const": "gold"}}
                    },
                    "then": {
                      "properties": {
                        "preferences": {
                          "properties": {
                            "themes": {
                              "minItems": 2
                            }
                          }
                        }
                      }
                    }
                  }
                ],
                "patternProperties": {
                  "^S_": {
                    "type": "string",
                    "minLength": 3
                  }
                }
                })");
        std::vector<std::string> ref_keys{"paths", "/pets/test", "get",
                                          "requestBody/content/application%2Fjson/schema"};
        validator_ = std::make_unique<BodyValidator>(schema_doc_, ref_keys);
    }

    rapidjson::Document schema_doc_;
    std::unique_ptr<BodyValidator> validator_;
};

TEST_F(TestBodyValidator, ValidJson)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark", "light"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::NONE);
    EXPECT_TRUE(error_msg.empty());
}

TEST_F(TestBodyValidator, InvalidParsing)
{
    std::string json_str = R"({
            "userId": 12345
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark", "light"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "parserError");
}
TEST_F(TestBodyValidator, InvalidIntegerType)
{
    std::string json_str = R"({
            "userId": "not-an-integer",
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark", "light"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "type");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/userId");
}

TEST_F(TestBodyValidator, InvalidStringType)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": 123,
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark", "light"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "type");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/username");
}

TEST_F(TestBodyValidator, InvalidArrayUniqueItems)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark", "dark", "light"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "uniqueItems");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/preferences/themes");
}

TEST_F(TestBodyValidator, InvalidIntegerMinimum)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 17,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "minimum");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/age");
}

TEST_F(TestBodyValidator, InvalidIntegerMaximim)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 101,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "maximum");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/age");
}

TEST_F(TestBodyValidator, InvalidRequired)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark"]
            },
            "createdAt": "2023-04-05T08:00:00Z",
            "S_example": "This is a pattern property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "required");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/address");
}

TEST_F(TestBodyValidator, InvalidPatternProp)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "gold",
            "preferences": {
              "newsletter": true,
              "themes": ["dark"]
            },
              "createdAt": "2023-04-01T14:30:00Z",
              "S_": "This should be invalid",
              "S_example": "Valid pattern property",
              "T_example": "Another invalid property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "additionalProperties");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#");
}

TEST_F(TestBodyValidator, InvalidEnum)
{
    std::string json_str = R"({
            "userId": 12345,
            "username": "johndoe_2023",
            "email": "john.doe@example.com",
            "age": 28,
            "phoneNumber": "+12345678901",
            "address": {
              "street": "123 Main St",
              "city": "Anytown",
              "state": "CA",
              "zipCode": "90210"
            },
            "subscriptionType": "invalid",
            "preferences": {
              "newsletter": true,
              "themes": ["dark"]
            },
              "createdAt": "2023-04-01T14:30:00Z",
              "S_": "This should be invalid",
              "S_example": "Valid pattern property",
              "T_example": "Another invalid property"
        })";
    std::string error_msg;
    EXPECT_EQ(validator_->Validate(json_str, error_msg), ValidationError::INVALID_BODY);
    EXPECT_FALSE(error_msg.empty());
    rapidjson::Document doc;
    doc.Parse(error_msg.c_str());
    EXPECT_FALSE(doc.HasParseError());
    EXPECT_EQ(std::string(doc["errorCode"].GetString()), "INVALID_BODY");
    EXPECT_EQ(std::string(doc["details"]["code"].GetString()), "enum");
    EXPECT_EQ(std::string(doc["details"]["instance"].GetString()), "#/subscriptionType");
}