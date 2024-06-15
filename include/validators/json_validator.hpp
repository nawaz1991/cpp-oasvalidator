/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef JSON_VALIDATOR_HPP
#define JSON_VALIDATOR_HPP

#include "validators/base_validator.hpp"

#include <mutex>
#include <rapidjson/schema.h>

class JsonValidator: public BaseValidator
{
private:
    rapidjson::SchemaDocument* schema_;
    rapidjson::SchemaValidator* validator_;
    std::mutex mutex_{};

    void CreateErrorMessages(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& errors,
                             const std::string& context, std::string& error_msg, bool recursive = false);
    void HandleError(const char* error_name,
                     const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& error,
                     const std::string& context, std::string& error_msg, bool recursive);
    static std::string GetString(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& val);

public:
    JsonValidator(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys,
                  ValidationError err_code);
    JsonValidator(const JsonValidator&) = delete;
    JsonValidator& operator=(const JsonValidator&) = delete;
    ValidationError Validate(const std::string& json_str, std::string& error_msg) override;
    ~JsonValidator() override;
};

#endif // JSON_VALIDATOR_HPP
