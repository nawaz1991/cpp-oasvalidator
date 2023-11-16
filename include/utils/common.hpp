/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <vector>

class ValidatorInitExc: public std::exception
{
private:
    std::string ex_msg_;

public:
    explicit ValidatorInitExc(std::string message)
        : ex_msg_(std::move(message))
    {
    }

    [[nodiscard]] const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override
    {
        return ex_msg_.c_str();
    }
};

#define CHECK_ERROR(err)                \
    if (ValidationError::NONE != err) { \
        return err;                     \
    }

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

enum class HttpMethod
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
    COUNT
};

enum class ParamStyle
{
    SIMPLE, // Path, query, header
    LABEL, // Path
    MATRIX, // Path
    FORM, // Query
    SPACE_DELIM, // Query
    PIPE_DELIM, // Query
    DEEP_OBJ, // Query
    CONTENT
};

enum class PrimitiveType
{
    BOOLEAN,
    INTEGER,
    NUMBER,
    STRING
};

enum class ExtendedType
{
    BOOLEAN,
    INTEGER,
    NUMBER,
    STRING,
    ARRAY,
    OBJECT
};

enum class SecurityType
{
    HTTP_BASIC,
    HTTP_BEARER,
    APIKEY_QUERY,
    APIKEY_HEADER,
    OAUTH2,
    OIDC
};

inline const char* Seek(const char* beg, const char* end, const char c)
{
    while (beg < end && *beg != c) {
        ++beg;
    }
    return beg;
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
}

#endif // COMMON_HPP
