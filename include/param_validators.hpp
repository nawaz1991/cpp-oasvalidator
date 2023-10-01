/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef PARAM_VALIDATOR_HPP
#define PARAM_VALIDATOR_HPP

#include "json_validator.hpp"

#include <functional>
#include <rapidjson/schema.h>
#include <unordered_map>
#include <utility>

using Deserializer = std::function<std::string(const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper)>;

class ParamValidator: public JsonValidator
{
private:
    struct ParamInfo
    {
        std::string name;
        bool required;
        const Deserializer& deserializer;
        std::unordered_map<std::string, ParamType> kv_map;
        const rapidjson::Value& schema;
    };

public:
    ParamValidator(const ParamInfo& param_info, const std::vector<std::string>& keys, ValidationError err_code);

    ValidationError Validate(const char* beg, const char* end, std::string& error_msg);
    bool IsRequired() const;
    ValidationError ErrorOnMissing(std::string& error_msg) const;
    ~ParamValidator() override = default;

protected:
    static ParamInfo GetParamInfo(const rapidjson::Value& param_val, const std::string& default_style, bool default_explode, bool default_required);

private:
    static const Deserializer kDeserializers[STYLE_COUNT][TYPE_COUNT][2];

    const std::string name_;
    const bool required_;
    const Deserializer& deserializer_;
    const std::unordered_map<std::string, ParamType> kv_map_;
    static const std::unordered_map<std::string, ParamStyle> kParamStyleMap;
    static const std::unordered_map<std::string, ParamType> kParamTypeMap;
};

class PathParamValidator final: public ParamValidator
{
public:
    explicit PathParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    ~PathParamValidator() override = default;
};

class QueryParamValidator final: public ParamValidator
{
public:
    explicit QueryParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    bool IsEmptyAllowed() const;
    ~QueryParamValidator() override = default;

private:
    bool empty_allowed_;
};

class HeaderParamValidator final: public ParamValidator
{
public:
    explicit HeaderParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    ~HeaderParamValidator() override = default;
};
#endif // PARAM_VALIDATOR_HPP
