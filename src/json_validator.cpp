/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "json_validator.hpp"

#include <sstream>

JsonValidator::JsonValidator(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys, ValidationError err_code)
    : BaseValidator(ref_keys, err_code)
{
    schema_ = new rapidjson::SchemaDocument(schema_val);
    if (schema_) {
        validator_ = new rapidjson::SchemaValidator(*schema_);
    }
}

ValidationError JsonValidator::Validate(const std::string& json_str, std::string& error_msg)
{
    rapidjson::Document doc;
    doc.Parse(json_str.c_str());
    if (doc.HasParseError()) {
        error_msg = err_header_ + R"("code":"parserError","description":")" + rapidjson::GetParseError_En(doc.GetParseError()) + R"(","offset":)" +
                    std::to_string(doc.GetErrorOffset()) + "}}";
        return code_on_error_;
    }

    if (!validator_) // No validator, no validation
    {
        return ValidationError::NONE;
    }

    if (doc.Accept(*this->validator_)) {
        validator_->Reset();
        return ValidationError::NONE;
    }

    error_msg.reserve(1024);
    error_msg = err_header_;
    CreateErrorMessages(validator_->GetError(), std::string(), error_msg);
    error_msg.append("}}");
    return code_on_error_;
}

void JsonValidator::CreateErrorMessages(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& errors, const std::string& context, std::string& error_msg,
                                        bool recursive)
{
    for (const auto& error_type : errors.GetObject()) {
        const char* error_name = error_type.name.GetString();
        const auto& error_content = error_type.value;

        if (error_content.IsArray()) {
            for (const auto& content : error_content.GetArray()) {
                HandleError(error_name, content, context, error_msg, recursive);
            }
        } else if (error_content.IsObject()) {
            HandleError(error_name, error_content, context, error_msg, recursive);
        }
    }
}

void JsonValidator::HandleError(const char* error_name, const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& error, const std::string& context,
                                std::string& error_msg, bool recursive)
{
    if (!error.ObjectEmpty()) {
        int code = error["errorCode"].GetInt();
        std::string message(GetValidateError_En(static_cast<rapidjson::ValidateErrorCode>(code)));

        for (const auto& insert : error.GetObject()) {
            std::string insert_name = "%" + std::string(insert.name.GetString());
            size_t insert_pos = message.find(insert_name);

            if (insert_pos != std::string::npos) {
                std::string insert_string;

                if (insert.value.IsArray()) {
                    for (const auto& item : insert.value.GetArray()) {
                        if (!insert_string.empty()) {
                            insert_string += ",";
                        }
                        insert_string += GetString(item);
                    }
                } else {
                    insert_string = GetString(insert.value);
                }

                message.replace(insert_pos, insert_name.length(), insert_string);
            }
        }

        if (recursive) {
            error_msg.push_back('{');
        }
        error_msg += R"("code":")" + std::string(error_name) + R"(",)" + R"("description":")" + message + R"(",)" + R"("instance":")" + error["instanceRef"].GetString() + R"(",)" +
                     R"("schema":")" + error["schemaRef"].GetString() + R"(")";

        if (!context.empty()) {
            error_msg += R"(,"context":")" + context + R"(")";
        }

        if (error.HasMember("errors")) {
            error_msg += R"(,"errors":[)";
            const auto& child_errors = error["errors"];

            if (child_errors.IsArray()) {
                for (const auto& child_error : child_errors.GetArray()) {
                    CreateErrorMessages(child_error, error_name, error_msg, true);
                }
            } else if (child_errors.IsObject()) {
                for (const auto& prop : child_errors.GetObject()) {
                    CreateErrorMessages(prop.value, error_name, error_msg, true);
                }
            }
            error_msg += "]";
        }
        if (recursive) {
            error_msg += "}";
        }
    }
}

std::string JsonValidator::GetString(const rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>& val)
{
    if (val.IsString()) {
        return val.GetString();
    }
    if (val.IsDouble()) {
        return std::to_string(val.GetDouble());
    }
    if (val.IsUint()) {
        return std::to_string(val.GetUint());
    }
    if (val.IsInt()) {
        return std::to_string(val.GetInt());
    }
    if (val.IsUint64()) {
        return std::to_string(val.GetUint64());
    }
    if (val.IsInt64()) {
        return std::to_string(val.GetInt64());
    }
    if (val.IsBool()) {
        return val.GetBool() ? "true" : "false";
    }
    if (val.IsFloat()) {
        return std::to_string(val.GetFloat());
    }

    return "";
}

JsonValidator::~JsonValidator()
{
#ifndef LUA_OAS_VALIDATOR // LUA manages garbage collection itself
    delete validator_;
    delete schema_;
#endif
}
