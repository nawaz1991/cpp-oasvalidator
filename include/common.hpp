/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <vector>

#define CHECK_ERROR(err)                \
    if (ValidationError::NONE != err) { \
        return err;                     \
    }

inline const char* Seek(const char* start, const char* end, const char c)
{
    while (start < end && *start != c) {
        ++start;
    }
    return start;
}

inline std::string EscapeSlash(const std::string& str)
{
    std::string escaped_str;
    for (char ch : str) {
        if (ch == '/') {
            escaped_str += "%2F";
        } else {
            escaped_str += ch;
        }
    }
    return escaped_str;
}

inline std::string JoinReference(const std::vector<std::string>& ref_keys)
{
    std::string reference = "#";
    for (const auto& ref_key : ref_keys) {
        reference += "/" + ref_key;
    }
    return reference;
};

struct ParamRange
{
    const char* beg;
    const char* end;
};

#ifndef VALIDATION_ERROR
#define VALIDATION_ERROR
enum class ValidationError
{
    NONE = 0,
    INVALID_METHOD = -1,
    INVALID_ROUTE = -2,
    INVALID_PATH_PARAM = -3,
    INVALID_QUERY_PARAM = -4,
    INVALID_HEADER_PARAM = -5,
    INVALID_BODY = -6,
    INVALID_RSP = -7
};
#endif

enum HttpMethod
{
    GET = 0,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    PATCH,
    CONNECT,
    TRACE,
    METHOD_COUNT
};

enum ParamStyle
{
    STYLE_SIMPLE, // Path, query, header
    STYLE_LABEL, // Path
    STYLE_MATRIX, // Path
    STYLE_FORM, // Query
    STYLE_SPACE_DELIM, // Query
    STYLE_PIPE_DELIM, // Query
    STYLE_DEEP_OBJ, // Query
    STYLE_COUNT
};

enum ParamType
{
    TYPE_PRIMITIVE,
    TYPE_STRING,
    TYPE_ARRAY_PRIMITIVE,
    TYPE_ARRAY_STRING,
    TYPE_OBJECT,
    TYPE_COUNT
};

#endif // COMMON_HPP
