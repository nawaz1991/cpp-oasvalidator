/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef CONTENT_DESERIALIZER_HPP
#define CONTENT_DESERIALIZER_HPP
#include "deserializers/base_deserializer.hpp"

class ContentDeserializer final: public BaseDeserializer
{
public:
    explicit ContentDeserializer(const std::string& param_name, char start, bool skip_name);

    std::string Deserialize(const char* beg, const char* const end) override;
    ~ContentDeserializer() override = default;
};

#endif // CONTENT_DESERIALIZER_HPP
