/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "oas_validator.hpp"

#include "oas_validator_imp.hpp"

OASValidator::OASValidator(const std::string& oas_specs)
    : impl_(new OASValidatorImp(oas_specs))
{
}

ValidationError OASValidator::ValidateRoute(const std::string& method, const std::string& http_path,
                                            std::string& error_msg)
{
    return impl_->ValidateRoute(method, http_path, error_msg);
}

ValidationError OASValidator::ValidateBody(const std::string& method, const std::string& http_path,
                                           const std::string& json_body, std::string& error_msg)
{
    return impl_->ValidateBody(method, http_path, json_body, error_msg);
}
ValidationError OASValidator::ValidatePathParam(const std::string& method, const std::string& http_path,
                                                std::string& error_msg)
{
    return impl_->ValidatePathParam(method, http_path, error_msg);
}

ValidationError OASValidator::ValidateQueryParam(const std::string& method, const std::string& http_path,
                                                 std::string& error_msg)
{
    return impl_->ValidateQueryParam(method, http_path, error_msg);
}

ValidationError OASValidator::ValidateHeaders(const std::string& method, const std::string& http_path,
                                              const std::unordered_map<std::string, std::string>& headers,
                                              std::string& error_msg)
{
    return impl_->ValidateHeaders(method, http_path, headers, error_msg);
}

ValidationError OASValidator::ValidateRequest(const std::string& method, const std::string& http_path,
                                              std::string& error_msg)
{
    return impl_->ValidateRequest(method, http_path, error_msg);
}

ValidationError OASValidator::ValidateRequest(const std::string& method, const std::string& http_path,
                                              const std::string& json_body, std::string& error_msg)
{
    return impl_->ValidateRequest(method, http_path, json_body, error_msg);
}

ValidationError OASValidator::ValidateRequest(const std::string& method, const std::string& http_path,
                                              const std::unordered_map<std::string, std::string>& headers,
                                              std::string& error_msg)
{
    return impl_->ValidateRequest(method, http_path, headers, error_msg);
}

ValidationError OASValidator::ValidateRequest(const std::string& method, const std::string& http_path,
                                              const std::string& json_body,
                                              const std::unordered_map<std::string, std::string>& headers,
                                              std::string& error_msg)
{
    return impl_->ValidateRequest(method, http_path, json_body, headers, error_msg);
}

OASValidator::~OASValidator()
{
    delete impl_;
};