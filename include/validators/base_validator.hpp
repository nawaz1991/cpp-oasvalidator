/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include "utils/common.hpp"

#include <string>
#include <unordered_map>

class BaseValidator
{
public:
    explicit BaseValidator(ValidationError err_code);
    explicit BaseValidator(const std::vector<std::string>& ref_keys, ValidationError err_code);

    virtual ValidationError Validate(const std::string& content, std::string& err_msg) = 0;
    std::string GetErrHeader() const;
    virtual ~BaseValidator() = default;

protected:
    ValidationError code_on_error_;
    std::string err_header_;

private:
    static const std::unordered_map<ValidationError, std::string> kErrHeaders;
};

#endif // VALIDATOR_HPP
