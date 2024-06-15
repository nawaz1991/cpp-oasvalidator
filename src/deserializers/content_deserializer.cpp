/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/content_deserializer.hpp"

ContentDeserializer::ContentDeserializer(const std::string& param_name, char start, bool skip_name)
    : BaseDeserializer(param_name, start, skip_name)
{
}

std::string ContentDeserializer::Deserialize(const char* beg, const char* const end)
{
    const char* cursor = beg;

    CheckNSkipStart(cursor);
    if (skip_name_) {
        CheckNSkipName(cursor, end);
        CheckNSkipChar(cursor, end, '=');
    }

    std::string ret;
    ret.reserve(static_cast<std::string::size_type>(end - cursor));

    while (cursor < end) {
        char c = *cursor++;
        switch (c) {
        case '%': {
            if (cursor + 1 >= end) {
                throw DeserializationException("Incomplete percent encoding for '" + param_name_ + "'");
            }
            const char dec1 = kHexLookupTable[static_cast<unsigned char>(*cursor++)];
            const char dec2 = kHexLookupTable[static_cast<unsigned char>(*cursor++)];
            if (dec1 < 0 || dec2 < 0) {
                throw DeserializationException("Invalid HEX character for '" + param_name_ + "'");
            }
            ret.push_back(static_cast<char>((dec1 << 4) | dec2));
        } break;

        case '+':
            ret += ' ';
            break;

        default:
            ret += c;
            break;
        }
    }

    CheckEnd(cursor, end);
    return ret;
}
