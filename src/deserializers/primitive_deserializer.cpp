/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#include "deserializers/primitive_deserializer.hpp"

PrimitiveDeserializer::PrimitiveDeserializer(const std::string& param_name, char start, bool skip_name,
                                             PrimitiveType param_type)
    : BaseDeserializer(param_name, start, skip_name)
    , param_type_(param_type)
{
}
std::string PrimitiveDeserializer::Deserialize(const char* beg, const char* const end)
{
    const char* cursor = beg;

    CheckNSkipStart(cursor);

    if (skip_name_) {
        CheckNSkipName(cursor, end);
        CheckNSkipChar(cursor, end, '=');
    }

    CheckData(cursor, end);

    std::string ret;
    ret.reserve(static_cast<size_t>(end - beg + 2));

    switch (param_type_) {
    case PrimitiveType::BOOLEAN:
        DeserializeBoolean(cursor, end, ret);
        break;

    case PrimitiveType::INTEGER:
        DeserializeInteger(cursor, end, ret);
        break;

    case PrimitiveType::NUMBER:
        DeserializeNumber(cursor, end, ret);
        break;

    case PrimitiveType::STRING:
        DeserializeString(cursor, end, ret);
        break;
    default:
        throw DeserializationException("Invalid primitive type for '" + param_name_ + "'");
    }

    CheckEnd(cursor, end);

    return ret;
}
