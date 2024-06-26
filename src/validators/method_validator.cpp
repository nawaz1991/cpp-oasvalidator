/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "validators/method_validator.hpp"

MethodValidator::MethodValidator()
    : BaseValidator(ValidationError::INVALID_METHOD)
{
}

ValidationError MethodValidator::Validate(const std::string& method, std::string& err_msg)
{
    if (kValidMethods.find(method) == kValidMethods.end()) {
        err_msg += err_header_ + R"("description": "Invalid HTTP method ')" + method + "'" + R"("}})";
        return ValidationError::INVALID_METHOD;
    }
    return ValidationError::NONE;
}

const std::unordered_set<std::string> MethodValidator::kValidMethods = {"GET",     "POST",    "PUT",     "DELETE",
                                                                        "HEAD",    "OPTIONS", "PATCH",   "CONNECT",
                                                                        "TRACE",   "get",     "post",    "put",
                                                                        "delete",  "head",    "options", "patch",
                                                                        "connect", "trace"};