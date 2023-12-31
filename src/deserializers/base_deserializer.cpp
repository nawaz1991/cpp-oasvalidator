/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/base_deserializer.hpp"

BaseDeserializer::BaseDeserializer(const std::string& param_name, char start, bool skip_name)
    : param_name_(param_name)
    , start_(start)
    , skip_name_(skip_name)
{
}

const std::array<char, 256> BaseDeserializer::kHexLookupTable = []() {
    std::array<char, 256> table{};
    for (int i = 0; i < 256; ++i) {
        table[i] = (i >= '0' && i <= '9') ? i - '0' : (i >= 'A' && i <= 'F') ? i - 'A' + 10 : (i >= 'a' && i <= 'f') ? i - 'a' + 10 : -1;
    }
    return table;
}();
