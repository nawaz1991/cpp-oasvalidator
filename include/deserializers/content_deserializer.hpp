/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

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
