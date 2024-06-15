/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef PARAM_VALIDATOR_HPP
#define PARAM_VALIDATOR_HPP

#include "deserializers/base_deserializer.hpp"
#include "validators/json_validator.hpp"
#include <rapidjson/schema.h>
#include <unordered_map>
#include <utility>

class ParamValidator: public JsonValidator
{
private:
    struct ParamInfo
    {
        std::string name;
        bool required;
        BaseDeserializer* deserializer;
        const rapidjson::Value& schema;
    };

public:
    ParamValidator(const ParamInfo& param_info, const std::vector<std::string>& ref_keys, ValidationError err_code);
    ParamValidator(const ParamValidator&) = delete;
    ParamValidator& operator=(const ParamValidator&) = delete;

    ValidationError ValidateParam(const char* beg, const char* end, std::string& error_msg);
    bool IsRequired() const;
    ValidationError ErrorOnMissing(std::string& error_msg) const;
    ~ParamValidator() override = default;

protected:
    static ParamInfo GetParamInfo(const rapidjson::Value& param_val, const std::string& default_style,
                                  bool default_explode, bool default_required,
                                  const std::vector<std::string>& ref_keys);

private:
    const std::string name_;
    const bool required_;
    BaseDeserializer* deserializer_;
};

class PathParamValidator final: public ParamValidator
{
public:
    explicit PathParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    PathParamValidator(const PathParamValidator&) = delete;
    PathParamValidator& operator=(const PathParamValidator&) = delete;
    ~PathParamValidator() override = default;
};

class QueryParamValidator final: public ParamValidator
{
public:
    explicit QueryParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    QueryParamValidator(const QueryParamValidator&) = delete;
    QueryParamValidator& operator=(const QueryParamValidator&) = delete;
    bool IsEmptyAllowed() const;
    ~QueryParamValidator() override = default;

private:
    bool empty_allowed_;
};

class HeaderParamValidator final: public ParamValidator
{
public:
    explicit HeaderParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& keys);
    HeaderParamValidator(const HeaderParamValidator&) = delete;
    HeaderParamValidator& operator=(const HeaderParamValidator&) = delete;
    ~HeaderParamValidator() override = default;
};
#endif // PARAM_VALIDATOR_HPP
