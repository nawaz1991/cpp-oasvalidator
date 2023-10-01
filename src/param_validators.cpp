/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "param_validators.hpp"

ParamValidator::ParamValidator(const ParamInfo& param_info, const std::vector<std::string>& ref_keys, ValidationError err_code)
    : JsonValidator(param_info.schema, ref_keys, err_code)
    , name_(param_info.name)
    , required_(param_info.required)
    , deserializer_(param_info.deserializer)
    , kv_map_(param_info.kv_map)
{
}

ValidationError ParamValidator::Validate(const char* beg, const char* const end, std::string& error_msg)
{
    return JsonValidator::Validate(deserializer_(beg, end, kv_map_), error_msg);
}

bool ParamValidator::IsRequired() const
{
    return required_;
}

ValidationError ParamValidator::ErrorOnMissing(std::string& error_msg) const
{
    error_msg = err_header_ + R"("description":"Missing required parameter '")" + name_ + R"('}})";
    return code_on_error_;
}

ParamValidator::ParamInfo ParamValidator::GetParamInfo(const rapidjson::Value& param_val, const std::string& default_style, bool default_explode, bool default_required)
{
    std::string name(param_val["name"].GetString());
    auto required(param_val.HasMember("required") ? param_val["required"].GetBool() : default_required);
    auto explode(param_val.HasMember("explode") ? param_val["explode"].GetBool() : default_explode);
    auto style(param_val.HasMember("style") ? param_val["style"].GetString() : default_style);
    std::string type("object"); // if schema is not provided, default object for content

    if (param_val.HasMember("schema") && param_val["schema"].HasMember("type")) {
        type = param_val["schema"]["type"].GetString();
        if ("array" == type) {
            if (param_val["schema"].HasMember("items") && param_val["schema"]["items"].HasMember("type")) {
                type += "_" + std::string(param_val["schema"]["items"]["type"].GetString());
            } else {
                type += "_string"; // TODO is it okay?
            }
        }
    }
    std::unordered_map<std::string, ParamType> kv_map;
    if ("object" == type && param_val.HasMember("schema") && param_val["schema"].HasMember("properties")) {
        for (const auto& prop : param_val["schema"]["properties"].GetObject()) {
            // Ensure each property has a "type" member, and it's a string
            if (prop.value.HasMember("type") && prop.value["type"].IsString()) {
                std::string prop_name(prop.name.GetString());
                std::string prop_type(prop.value["type"].GetString());
                if ("array" == prop_type) {
                    if (prop.value.HasMember("items") && prop.value.HasMember("type")) {
                        type += "_" + std::string(prop.value["type"].GetString());
                    } else {
                        type += "_string"; // TODO is it okay?
                    }
                }
                kv_map.emplace(name, kParamTypeMap.at(prop_type));
            }
        }
    }
    if (param_val.HasMember("content") && param_val["content"].HasMember("application/json") && param_val["content"]["application/json"].HasMember("schema")) {
        style = "simple";
        type = "boolean";
        explode = false;
    }

    static rapidjson::Value empty_value(rapidjson::kObjectType); // TODO handle this

    return {name, required, kDeserializers[kParamStyleMap.at(style)][kParamTypeMap.at(type)][explode], kv_map,
            param_val.HasMember("schema")
                ? param_val["schema"]
                : (param_val.HasMember("content") && param_val["content"].HasMember("application/json") && param_val["content"]["application/json"].HasMember("schema")
                       ? param_val["content"]["application/json"]["schema"]
                       : empty_value)};
}

PathParamValidator::PathParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "simple", false, true), ref_keys, ValidationError::INVALID_PATH_PARAM)
{
}

QueryParamValidator::QueryParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "form", true, false), ref_keys, ValidationError::INVALID_QUERY_PARAM)
    , empty_allowed_(param_val.HasMember("allowEmptyValue") && param_val["allowEmptyValue"].GetBool())
{
}

bool QueryParamValidator::IsEmptyAllowed() const
{
    return empty_allowed_;
}

HeaderParamValidator::HeaderParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "simple", false, false), ref_keys, ValidationError::INVALID_PATH_PARAM)
{
}

const std::unordered_map<std::string, ParamStyle> ParamValidator::kParamStyleMap = {{"simple", STYLE_SIMPLE},
                                                                                    {"label", STYLE_LABEL},
                                                                                    {"matrix", STYLE_MATRIX},
                                                                                    {"form", STYLE_FORM},
                                                                                    {"spaceDelimited", STYLE_SPACE_DELIM},
                                                                                    {"pipeDelimited", STYLE_PIPE_DELIM},
                                                                                    {"deepObject", STYLE_DEEP_OBJ}};

const std::unordered_map<std::string, ParamType> ParamValidator::kParamTypeMap = {{"boolean", TYPE_PRIMITIVE},
                                                                                  {"integer", TYPE_PRIMITIVE},
                                                                                  {"number", TYPE_PRIMITIVE},
                                                                                  {"string", TYPE_STRING},
                                                                                  {"object", TYPE_OBJECT},
                                                                                  {"array_boolean", TYPE_ARRAY_PRIMITIVE},
                                                                                  {"array_integer", TYPE_ARRAY_PRIMITIVE},
                                                                                  {"array_number", TYPE_ARRAY_PRIMITIVE},
                                                                                  {"array_string", TYPE_ARRAY_STRING}};