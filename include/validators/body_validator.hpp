/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

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
