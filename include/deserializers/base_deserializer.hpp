/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef BASE_DESERIALIZER_HPP
#define BASE_DESERIALIZER_HPP

#include "utils/common.hpp"

#include <array>
#include <cstring>
#include <stdexcept>
#include <string>

class DeserializationException: public std::exception
{
private:
    std::string ex_msg_;

public:
    explicit DeserializationException(const std::string& message)
        : ex_msg_(R"("description":")" + message + R"(")")
    {
    }

    [[nodiscard]] const char* what() const noexcept override
    {
        return ex_msg_.c_str();
    }
};

class BaseDeserializer
{
public:
    explicit BaseDeserializer(const std::string& param_name, char start, bool skip_name);

    virtual std::string Deserialize(const char* beg, const char* const end) = 0;
    virtual ~BaseDeserializer() = default;

protected:
    const std::string param_name_;
    const char start_;
    const bool skip_name_;

    static const std::array<char, 256> kHexLookupTable;

    // Utilities

    inline void CheckNSkipStart(const char*& cursor) const
    {
        if (start_) {
            if (start_ == *cursor) {
                ++cursor;
            } else {
                throw DeserializationException("Parameter '" + param_name_ + "' should start with '" + std::string(1, start_) + "'");
            }
        }
    }

    inline void CheckNSkipChar(const char*& cursor, const char* const end, const char c) const
    {
        if (cursor < end) {
            if (c == *cursor) {
                ++cursor;
            } else {
                throw DeserializationException("Invalid serialization of ' for parameter '" + param_name_ + "'");
            }
        }
    }

    inline void CheckNSkipName(const char*& cursor, const char* const end) const
    {
        if (std::distance(cursor, end) < static_cast<long>(param_name_.size()) || !std::equal(param_name_.begin(), param_name_.end(), cursor)) {
            throw DeserializationException("Parameter name mismatch for the parameter '" + param_name_ + "'");
        }
        cursor += param_name_.size();
    }

    inline void CheckData(const char*& cursor, const char* const end) const
    {
        if (cursor >= end) {
            throw DeserializationException("Parameter '" + param_name_ + "' has no data");
        }
    }

    inline void DeserializeBoolean(const char*& cursor, const char* const end, std::string& ret) const
    {
        constexpr std::size_t true_size = sizeof("true") - 1;
        constexpr std::size_t false_size = sizeof("false") - 1;

        if ((cursor + true_size) <= end && std::strncmp(cursor, "true", true_size) == 0) {
            ret.append("true");
            cursor += true_size;
        } else if ((cursor + false_size) <= end && std::strncmp(cursor, "false", false_size) == 0) {
            ret.append("false");
            cursor += false_size;
        } else {
            throw DeserializationException("Invalid `boolean` value for parameter `" + param_name_ + "`");
        }
    }

    inline void DeserializeInteger(const char*& cursor, const char* const end, std::string& ret) const
    {
        const char* start_cursor = cursor;
        if (cursor < end && '-' == *cursor) {
            ++cursor;
        }
        while (cursor < end && isdigit(*cursor)) {
            ++cursor;
        }
        if (cursor > start_cursor && (cursor - start_cursor != 1 || *start_cursor != '-')) {
            ret.append(start_cursor, cursor);
        } else {
            throw DeserializationException("Invalid 'integer' format for '" + param_name_ + "'");
        }
    }

    inline void DeserializeNumber(const char*& cursor, const char* const end, std::string& ret) const
    {
        const char* start_cursor = cursor;
        bool has_decimal_point = false;
        if (cursor < end && '-' == *cursor) {
            ++cursor;
        }
        while (cursor < end && (isdigit(*cursor) || *cursor == '.')) {
            if (*cursor == '.' && has_decimal_point) {
                throw DeserializationException("Multiple '.' in number for '" + param_name_ + "'");
            }
            has_decimal_point |= (*cursor == '.');
            ++cursor;
        }
        if (cursor > start_cursor && (cursor - start_cursor != 1 || (*start_cursor != '-' && *start_cursor != '.')) && (!has_decimal_point || *(cursor - 1) != '.')) {
            ret.append(start_cursor, cursor);
        } else {
            throw DeserializationException("Invalid 'number' format for '" + param_name_ + "'");
        }
    }

    inline void DeserializeString(const char*& cursor, const char* const end, std::string& ret) const
    {
        ret.push_back('"');
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
                ret.push_back((dec1 << 4) | dec2);
            } break;

            case '+':
                ret += ' ';
                break;

            default:
                ret += c;
                break;
            }
        }
        ret.push_back('"');
    }

    inline void DeserializeString(const char*& cursor, const char* const end, const char terminator, std::string& ret) const
    {
        ret.push_back('"');
        while (cursor < end && *cursor != terminator) {
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
                ret.push_back((dec1 << 4) | dec2);
            } break;

            case '+':
                ret += ' ';
                break;

            default:
                ret += c;
                break;
            }
        }
        ret.push_back('"');
    }

    inline void CheckEnd(const char*& cursor, const char* const end) const
    {
        if (cursor != end) {
            throw DeserializationException("Invalid serialization of parameter '" + param_name_ + "'");
        }
    }
};

#endif // BASE_DESERIALIZER_HPP
