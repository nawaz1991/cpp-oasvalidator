/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef ARRAY_DESERIALIZER_HPP
#define ARRAY_DESERIALIZER_HPP

#include "deserializers/base_deserializer.hpp"

class ArrayDeserializer final: public BaseDeserializer
{
public:
    explicit ArrayDeserializer(const std::string& param_name, char start, bool skip_name, PrimitiveType items_type,
                               char separator, bool has_running_name, bool has_20_separator);

    std::string Deserialize(const char* beg, const char* const end) override;
    ~ArrayDeserializer() override = default;

private:
    const PrimitiveType items_type_;
    const char separator_;
    const bool has_running_name_;
    const bool has_20_separator_; // style=spaceDelimited, explode=true separator is %20

    inline void CheckElementData(const char*& cursor, const char* const end) const
    {
        if (cursor >= end) {
            throw DeserializationException("Data for item of parameter '" + param_name_ + "' is missing");
        }
    }

    inline void CheckSeparator(const char*& cursor, const char* const end) const
    {
        if (cursor < end) {
            if (has_20_separator_) {
                CheckNSkipChar(cursor, end, '%');
                CheckNSkipChar(cursor, end, '2');
                CheckNSkipChar(cursor, end, '0');
            } else {
                CheckNSkipChar(cursor, end, separator_);
            }
            CheckElementData(cursor, end);
        }
    }

    inline void DeserializeBooleanArray(const char*& cursor, const char* const end, std::string& ret) const
    {
        while (cursor < end) {
            if (has_running_name_) {
                CheckNSkipName(cursor, end);
                CheckNSkipChar(cursor, end, '=');
                CheckElementData(cursor, end);
            }
            DeserializeBoolean(cursor, end, ret);
            CheckSeparator(cursor, end);
            ret.push_back(',');
        }
    }

    inline void DeserializeIntegerArray(const char*& cursor, const char* const end, std::string& ret) const
    {
        while (cursor < end) {
            if (has_running_name_) {
                CheckNSkipName(cursor, end);
                CheckNSkipChar(cursor, end, '=');
                CheckElementData(cursor, end);
            }
            DeserializeInteger(cursor, end, ret);
            CheckSeparator(cursor, end);
            ret.push_back(',');
        }
    }

    inline void DeserializeNumberArray(const char*& cursor, const char* const end, std::string& ret) const
    {
        while (cursor < end) {
            if (has_running_name_) {
                CheckNSkipName(cursor, end);
                CheckNSkipChar(cursor, end, '=');
                CheckElementData(cursor, end);
            }
            DeserializeNumber(cursor, end, ret);
            CheckSeparator(cursor, end);
            ret.push_back(',');
        }
    }

    inline void DeserializeStringArray(const char*& cursor, const char* const end, std::string& ret) const
    {
        while (cursor < end) {
            if (has_running_name_) {
                CheckNSkipName(cursor, end);
                CheckNSkipChar(cursor, end, '=');
                CheckElementData(cursor, end);
            }
            DeserializeString(cursor, end, separator_, ret);
            CheckSeparator(cursor, end);
            ret.push_back(',');
        }
    }
};

#endif // ARRAY_DESERIALIZER_HPP
