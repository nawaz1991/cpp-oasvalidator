/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

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

    virtual ValidationError ValidateJson(const std::string& content, std::string& err_msg) = 0;
    std::string GetErrHeader() const;
    virtual ~BaseValidator() = default;

protected:
    ValidationError code_on_error_;
    std::string err_header_;

    void InitErrHeader();

private:
    static const std::unordered_map<ValidationError, std::string> kErrHeaders;
};

#endif // VALIDATOR_HPP
