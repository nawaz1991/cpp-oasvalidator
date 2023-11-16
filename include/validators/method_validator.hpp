/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef METHOD_VALIDATOR_HPP
#define METHOD_VALIDATOR_HPP

#include "validators/base_validator.hpp"

#include <unordered_set>

class MethodValidator: public BaseValidator
{
public:
    MethodValidator();
    ValidationError ValidateJson(const std::string& method, std::string& err_msg) override;

private:
    static const std::unordered_set<std::string> kValidMethods;
};

#endif // METHOD_VALIDATOR_HPP
