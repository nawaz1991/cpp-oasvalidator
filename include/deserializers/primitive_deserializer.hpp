/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef PRIMITIVE_DESERIALIZER_HPP
#define PRIMITIVE_DESERIALIZER_HPP
#include "deserializers/base_deserializer.hpp"

class PrimitiveDeserializer final: public BaseDeserializer
{
public:
    explicit PrimitiveDeserializer(const std::string& param_name, char start, bool skip_name, PrimitiveType param_type);

    std::string Deserialize(const char* beg, const char* const end) override;
    ~PrimitiveDeserializer() override = default;

private:
    const PrimitiveType param_type_;
};

#endif // PRIMITIVE_DESERIALIZER_HPP
