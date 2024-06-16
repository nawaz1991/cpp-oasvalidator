/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

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
