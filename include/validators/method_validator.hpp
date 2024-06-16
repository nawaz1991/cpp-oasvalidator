/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef METHOD_VALIDATOR_HPP
#define METHOD_VALIDATOR_HPP

#include "validators/base_validator.hpp"

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
