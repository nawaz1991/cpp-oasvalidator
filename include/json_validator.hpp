/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef JSON_VALIDATOR_HPP
#define JSON_VALIDATOR_HPP

#include "base_validator.hpp"

#include <rapidjson/schema.h>

class JsonValidator: public BaseValidator
{
private:
    rapidjson::SchemaDocument* schema_ = nullptr;
    rapidjson::SchemaValidator* validator_ = nullptr;

    void CreateErrorMessages(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& errors, const std::string& context, std::string& error_msg,
                             bool recursive = false);
    void HandleError(const char* error_name, const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& error, const std::string& context, std::string& error_msg,
                     bool recursive);
    static std::string GetString(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& val);

public:
    JsonValidator(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys, ValidationError err_code);
    ValidationError Validate(const std::string& json_str, std::string& error_msg) override;
    ~JsonValidator() override;
};

#endif // JSON_VALIDATOR_HPP
