/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "deserializers/array_deserializer.hpp"

ArrayDeserializer::ArrayDeserializer(const std::string& param_name, char start, bool skip_name,
                                     PrimitiveType items_type, char separator, bool has_running_name,
                                     bool has_20_separator)
    : BaseDeserializer(param_name, start, skip_name)
    , items_type_(items_type)
    , separator_(separator)
    , has_running_name_(has_running_name)
    , has_20_separator_(has_20_separator)
{
}

std::string ArrayDeserializer::Deserialize(const char* beg, const char* const end)
{
    const char* cursor = beg;

    CheckNSkipStart(cursor);

    if (skip_name_ && !has_running_name_) {
        CheckNSkipName(cursor, end);
        CheckNSkipChar(cursor, end, '=');
    }

    CheckData(cursor, end);

    std::string ret;
    ret.reserve(static_cast<size_t>(end - beg + 64));
    ret.push_back('[');

    switch (items_type_) {
    case PrimitiveType::BOOLEAN:
        DeserializeBooleanArray(cursor, end, ret);
        break;

    case PrimitiveType::INTEGER:
        DeserializeIntegerArray(cursor, end, ret);
        break;

    case PrimitiveType::NUMBER:
        DeserializeNumberArray(cursor, end, ret);
        break;

    case PrimitiveType::STRING:
        DeserializeStringArray(cursor, end, ret);
        break;

    default:
        break;
    }

    CheckEnd(cursor, end);

    if (',' == ret.back()) {
        ret.back() = ']';
    } else {
        ret.push_back(']');
    }

    return ret;
}
