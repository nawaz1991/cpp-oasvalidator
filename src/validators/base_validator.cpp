/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "validators/base_validator.hpp"

BaseValidator::BaseValidator(ValidationError err_code)
    : code_on_error_(err_code)
    , err_header_(kErrHeaders.at(err_code))
{
}

BaseValidator::BaseValidator(const std::vector<std::string>& ref_keys, ValidationError err_code)
    : code_on_error_(err_code)
    , err_header_(kErrHeaders.at(err_code))
{
    err_header_ += R"("specRef":")" + JoinReference(ref_keys) + R"(",)";
}

std::string BaseValidator::GetErrHeader() const
{
    return err_header_;
}

const std::unordered_map<ValidationError, std::string> BaseValidator::kErrHeaders = {
    {ValidationError::NONE, "NONE"},
    {ValidationError::INVALID_METHOD, R"({"errorCode":"INVALID_METHOD","details":{)"},
    {ValidationError::INVALID_ROUTE, R"({"errorCode":"INVALID_ROUTE","details":{)"},
    {ValidationError::INVALID_PATH_PARAM, R"({"errorCode":"INVALID_PATH_PARAM","details":{)"},
    {ValidationError::INVALID_QUERY_PARAM, R"({"errorCode":"INVALID_QUERY_PARAM","details":{)"},
    {ValidationError::INVALID_HEADER_PARAM, R"({"errorCode":"INVALID_HEADER_PARAM","details":{)"},
    {ValidationError::INVALID_BODY, R"({"errorCode":"INVALID_BODY","details":{)"},
    {ValidationError::INVALID_RSP, R"({"errorCode":"INVALID_RSP","details":{)"}};
