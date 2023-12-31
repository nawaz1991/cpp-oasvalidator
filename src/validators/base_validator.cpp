/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "validators/base_validator.hpp"

BaseValidator::BaseValidator(ValidationError err_code)
    : code_on_error_(err_code)
{
    InitErrHeader();
}

BaseValidator::BaseValidator(const std::vector<std::string>& ref_keys, ValidationError err_code)
    : code_on_error_(err_code)
{
    InitErrHeader();
    err_header_ += R"("specRef":")" + JoinReference(ref_keys) + R"(",)";
}

std::string BaseValidator::GetErrHeader() const
{
    return err_header_;
}

void BaseValidator::InitErrHeader()
{
    err_header_ = kErrHeaders.at(code_on_error_);
}

const std::unordered_map<ValidationError, std::string> BaseValidator::kErrHeaders = {{ValidationError::NONE, "NONE"},
                                                                                     {ValidationError::INVALID_METHOD, R"({"errorCode":"INVALID_METHOD","details":{)"},
                                                                                     {ValidationError::INVALID_ROUTE, R"({"errorCode":"INVALID_ROUTE","details":{)"},
                                                                                     {ValidationError::INVALID_PATH_PARAM, R"({"errorCode":"INVALID_PATH_PARAM","details":{)"},
                                                                                     {ValidationError::INVALID_QUERY_PARAM, R"({"errorCode":"INVALID_QUERY_PARAM","details":{)"},
                                                                                     {ValidationError::INVALID_HEADER_PARAM, R"({"errorCode":"INVALID_HEADER_PARAM","details":{)"},
                                                                                     {ValidationError::INVALID_BODY, R"({"errorCode":"INVALID_BODY","details":{)"},
                                                                                     {ValidationError::INVALID_RSP, R"({"errorCode":"INVALID_RSP","details":{)"}};
