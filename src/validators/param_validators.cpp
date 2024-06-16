/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "validators/param_validators.hpp"
#include "deserializers/array_deserializer.hpp"
#include "deserializers/content_deserializer.hpp"
#include "deserializers/object_deserializer.hpp"
#include "deserializers/primitive_deserializer.hpp"

namespace {
inline char GetStartChar(ParamStyle param_style)
{
    switch (param_style) {
    case ParamStyle::LABEL:
        return '.';
    case ParamStyle::MATRIX:
        return ';';
    default:
        return '\0';
    }
}

inline bool HasNameAtStart(const std::string& in, ParamStyle style, bool explode, ExtendedType type)
{
    switch (style) {
    case ParamStyle::SIMPLE:
    case ParamStyle::LABEL:
        return false;
    case ParamStyle::MATRIX:
    case ParamStyle::FORM:
    case ParamStyle::SPACE_DELIM:
    case ParamStyle::PIPE_DELIM:
    case ParamStyle::DEEP_OBJ:
        return (ExtendedType::ARRAY != type && ExtendedType::OBJECT != type) || !explode;
    case ParamStyle::CONTENT:
        return "query" == in;
    default:
        return false;
    }
}

inline char GetArrayItemsSeparator(ParamStyle param_style, bool explode)
{
    if (explode) {
        switch (param_style) {
        case ParamStyle::LABEL:
            return '.';
        case ParamStyle::MATRIX:
            return ';';
        case ParamStyle::FORM:
        case ParamStyle::SPACE_DELIM:
        case ParamStyle::PIPE_DELIM:
            return '&';
        default:
            return ',';
        }
    } else {
        if (ParamStyle::SPACE_DELIM == param_style) {
            return '%';
        } else if (ParamStyle::PIPE_DELIM == param_style) {
            return '|';
        }
    }
    return ',';
}
inline bool HasArrayRunningName(ParamStyle param_style, bool explode)
{
    if (explode) {
        switch (param_style) {
        case ParamStyle::MATRIX:
        case ParamStyle::FORM:
        case ParamStyle::SPACE_DELIM:
        case ParamStyle::PIPE_DELIM:
            return true;
        default:
            return false;
        }
    }
    return false;
}

inline char GetObjKVSep(bool explode)
{
    if (explode) {
        return '=';
    }
    return ',';
}

inline char GetObjVKSep(ParamStyle param_style, bool explode)
{
    if (explode) {
        switch (param_style) {
        case ParamStyle::LABEL:
            return '.';
        case ParamStyle::MATRIX:
            return ';';
        case ParamStyle::FORM:
        case ParamStyle::DEEP_OBJ:
            return '&';
        default:
            return ',';
        }
    }
    return ',';
}

const std::unordered_map<std::string, ParamStyle> PARAM_STYLE_MAP = {{"simple", ParamStyle::SIMPLE},
                                                                     {"label", ParamStyle::LABEL},
                                                                     {"matrix", ParamStyle::MATRIX},
                                                                     {"form", ParamStyle::FORM},
                                                                     {"spaceDelimited", ParamStyle::SPACE_DELIM},
                                                                     {"pipeDelimited", ParamStyle::PIPE_DELIM},
                                                                     {"deepObject", ParamStyle::DEEP_OBJ},
                                                                     {"content", ParamStyle::CONTENT}};

const std::unordered_map<std::string, PrimitiveType> PRIMITIVE_TYPE_MAP = {{"boolean", PrimitiveType::BOOLEAN},
                                                                           {"integer", PrimitiveType::INTEGER},
                                                                           {"number", PrimitiveType::NUMBER},
                                                                           {"string", PrimitiveType::STRING}};

const std::unordered_map<std::string, ExtendedType> EXTENDED_TYPE_MAP = {{"boolean", ExtendedType::BOOLEAN},
                                                                         {"integer", ExtendedType::INTEGER},
                                                                         {"number", ExtendedType::NUMBER},
                                                                         {"string", ExtendedType::STRING},
                                                                         {"array", ExtendedType::ARRAY},
                                                                         {"object", ExtendedType::OBJECT}};

ObjKTMap GetKTMap(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
{
    ObjKTMap kt_map;
    for (const auto& prop : param_val["schema"]["properties"].GetObject()) {
        // Ensure each property has a "type" member, and it's a string
        std::string prop_name(prop.name.GetString());
        if (prop.value.HasMember("type")) {

            std::string prop_type(prop.value["type"].GetString());
            try {
                kt_map.emplace("\"" + prop_name + "\"", PRIMITIVE_TYPE_MAP.at(prop_type));
            } catch (const std::out_of_range&) {
                throw ValidatorInitExc("Type '" + prop_type + "' of property '" + prop_name + "' of parameter '" +
                                       JoinReference(ref_keys) + "' is not supported");
            }
        } else {
            throw ValidatorInitExc("Type id missing for property '" + prop_name + "' of parameter '" +
                                   JoinReference(ref_keys) + "'");
        }
    }
    return kt_map;
}

BaseDeserializer* GetDeserializer(const rapidjson::Value& param_val, const std::string& default_style,
                                  bool default_explode, const std::vector<std::string>& ref_keys)
{
    std::string param_name(param_val["name"].GetString());
    std::string in(param_val["in"].GetString());
    auto explode(param_val.HasMember("explode") ? param_val["explode"].GetBool() : default_explode);
    auto param_style =
        PARAM_STYLE_MAP.at(param_val.HasMember("style") ? param_val["style"].GetString() : default_style);

    auto start = GetStartChar(param_style);

    if (param_val.HasMember("schema") && param_val["schema"].HasMember("type")) {
        std::string type(param_val["schema"]["type"].GetString());
        auto skip_name = HasNameAtStart(in, param_style, explode, EXTENDED_TYPE_MAP.at(type));
        switch (EXTENDED_TYPE_MAP.at(type)) {
        case ExtendedType::BOOLEAN:
        case ExtendedType::INTEGER:
        case ExtendedType::NUMBER:
        case ExtendedType::STRING:
            return new PrimitiveDeserializer(param_name, start, skip_name, PRIMITIVE_TYPE_MAP.at(type));
        case ExtendedType::ARRAY: {
            if (param_val["schema"].HasMember("items") && param_val["schema"]["items"].HasMember("type")) {
                auto items_type_s(std::string(param_val["schema"]["items"]["type"].GetString()));
                try {
                    auto items_type = PRIMITIVE_TYPE_MAP.at(items_type_s);
                    auto separator = GetArrayItemsSeparator(param_style, explode);
                    auto has_running_name = HasArrayRunningName(param_style, explode);
                    bool has_20_separator(ParamStyle::SPACE_DELIM == param_style && !explode);
                    return new ArrayDeserializer(param_name, start, skip_name, items_type, separator, has_running_name,
                                                 has_20_separator);
                } catch (const std::out_of_range&) {
                    throw ValidatorInitExc("Type '" + items_type_s + "' of items of array parameter '" +
                                           JoinReference(ref_keys) + "' is not supported");
                }
            } else {
                throw ValidatorInitExc("Array parameter '" + JoinReference(ref_keys) + "' does not have type of items");
            }
        }
        case ExtendedType::OBJECT: {
            auto kv_separator = GetObjKVSep(explode);
            auto vk_separator = GetObjVKSep(param_style, explode);
            auto is_deep_obj(ParamStyle::DEEP_OBJ == param_style);
            auto kt_map = GetKTMap(param_val, ref_keys);
            return new ObjectDeserializer(param_name, start, skip_name, kv_separator, vk_separator, is_deep_obj,
                                          kt_map);
        }
        default:
            throw ValidatorInitExc("Invalid type of parameter: " + JoinReference(ref_keys));
        }
    } else if (param_val.HasMember("content")) {
        auto skip_name = HasNameAtStart(in, ParamStyle::CONTENT, explode, ExtendedType::OBJECT);
        return new ContentDeserializer(param_name, start, skip_name);
    }
    throw ValidatorInitExc("Invalid type of parameter: " + JoinReference(ref_keys));
}
} // namespace

ParamValidator::ParamValidator(const ParamInfo& param_info, const std::vector<std::string>& ref_keys,
                               ValidationError err_code)
    : JsonValidator(param_info.schema, ref_keys, err_code)
    , name_(param_info.name)
    , required_(param_info.required)
    , deserializer_(param_info.deserializer)
{
}

ValidationError ParamValidator::ValidateParam(const char* beg, const char* end, std::string& error_msg)
{
    try {
        auto ret = deserializer_->Deserialize(beg, end);
        return JsonValidator::Validate(ret, error_msg);
    } catch (const DeserializationException& exc) {
        error_msg = err_header_ + exc.what() + "}}";
        return code_on_error_;
    }
}

bool ParamValidator::IsRequired() const
{
    return required_;
}

ValidationError ParamValidator::ErrorOnMissing(std::string& error_msg) const
{
    error_msg = err_header_ + R"("description":"Missing required parameter ')" + name_ + R"('"}})";
    return code_on_error_;
}

ParamValidator::ParamInfo ParamValidator::GetParamInfo(const rapidjson::Value& param_val,
                                                       const std::string& default_style, bool default_explode,
                                                       bool default_required, const std::vector<std::string>& ref_keys)
{
    std::string name(param_val["name"].GetString());
    auto required(param_val.HasMember("required") ? param_val["required"].GetBool() : default_required);

    if (param_val.HasMember("schema")) {
        return {name, required, GetDeserializer(param_val, default_style, default_explode, ref_keys),
                param_val["schema"]};
    } else if (param_val.HasMember("content") && param_val["content"].HasMember("application/json") &&
               param_val["content"]["application/json"].HasMember("schema")) {
        return {name, required, GetDeserializer(param_val, default_style, default_explode, ref_keys),
                param_val["content"]["application/json"]["schema"]};
    } else {
        throw ValidatorInitExc("Cannot generate deserializer for parameter: " + JoinReference(ref_keys));
    }
}

PathParamValidator::PathParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "simple", false, true, ref_keys), ref_keys,
                     ValidationError::INVALID_PATH_PARAM)
{
}

QueryParamValidator::QueryParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "form", true, false, ref_keys), ref_keys,
                     ValidationError::INVALID_QUERY_PARAM)
    , empty_allowed_(param_val.HasMember("allowEmptyValue") && param_val["allowEmptyValue"].GetBool())
{
}

bool QueryParamValidator::IsEmptyAllowed() const
{
    return empty_allowed_;
}

HeaderParamValidator::HeaderParamValidator(const rapidjson::Value& param_val, const std::vector<std::string>& ref_keys)
    : ParamValidator(ParamValidator::GetParamInfo(param_val, "simple", false, false, ref_keys), ref_keys,
                     ValidationError::INVALID_HEADER_PARAM)
{
}