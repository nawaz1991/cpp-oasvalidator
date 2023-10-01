/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef METHOD_VALIDATOR_HPP
#define METHOD_VALIDATOR_HPP

#include "base_validator.hpp"

#include <unordered_set>

class MethodValidator: public BaseValidator
{
public:
    MethodValidator();
    ValidationError Validate(const std::string& method, std::string& err_msg) override;

private:
    static const std::unordered_set<std::string> kValidMethods;
};

#endif // METHOD_VALIDATOR_HPP
