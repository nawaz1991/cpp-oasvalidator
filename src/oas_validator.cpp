/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "oas_validator.hpp"

#include "oas_validator_imp.hpp"

OASValidator::OASValidator(const std::string& oas_specs,
                           const std::unordered_map<std::string, std::unordered_set<std::string>>& method_map)
    : impl_(new OASValidatorImp(oas_specs, method_map))
{
}

OASValidator::OASValidator(const OASValidator& other)
    : impl_(new OASValidatorImp(*other.impl_))
{
}

OASValidator& OASValidator::operator=(const OASValidator& other)
{
    if (this == &other) {
        return *this;
    }

    delete impl_;
    impl_ = new OASValidatorImp(*other.impl_);
    return *this;
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