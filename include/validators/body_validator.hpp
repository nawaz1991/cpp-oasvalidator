/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef BODY_VALIDATOR_HPP
#define BODY_VALIDATOR_HPP

#include "validators/json_validator.hpp"

class BodyValidator: public JsonValidator
{
public:
    explicit BodyValidator(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys)
        : JsonValidator(schema_val, ref_keys, ValidationError::INVALID_BODY)
    {
    }
};

#endif // BODY_VALIDATOR_HPP
