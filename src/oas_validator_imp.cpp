/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "oas_validator_imp.hpp"

#include <fstream>

OASValidatorImp::OASValidatorImp(const std::string& specs_path)
{
    if (specs_path.empty()) {
        throw OASValidatorException("OAS path is empty");
    }

    std::ifstream ifs(specs_path);
    if (!ifs.is_open()) {
        throw OASValidatorException("Cannot open OAS file: " + specs_path);
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    rapidjson::Document doc;
    doc.Parse(content.c_str());
    ifs.close();
    if (doc.HasParseError()) {
        throw OASValidatorException("Unable to parse specs: " + specs_path);
    }
    // TODO check schema

    const rapidjson::Value& paths = doc["paths"];
    std::vector<std::string> ref_keys;
    ref_keys.emplace_back("paths");
    for (auto path_itr = paths.MemberBegin(); path_itr != paths.MemberEnd(); ++path_itr) {
        std::string path(path_itr->name.GetString());
        ref_keys.emplace_back(EscapeSlash(path));
        const rapidjson::Value& methods = path_itr->value;
        for (auto method_itr = methods.MemberBegin(); method_itr != methods.MemberEnd(); ++method_itr) {
            ref_keys.emplace_back(method_itr->name.GetString());
            auto method(kStringToMethod.at(method_itr->name.GetString()));
            auto& per_method_validator = oas_validators_[method];
            auto& per_path_validator = per_method_validator.per_path_validators;

            if ((method_itr->value.HasMember("requestBody")) && (method_itr->value["requestBody"].HasMember("content")) &&
                (method_itr->value["requestBody"]["content"].HasMember("application/json")) &&
                (method_itr->value["requestBody"]["content"]["application/json"].HasMember("schema"))) { //  if "method+path" has json body
                ref_keys.emplace_back("requestBody/content/application%2Fjson/schema");
                per_path_validator.emplace(path, new ValidatorsStore(method_itr->value["requestBody"]["content"]["application/json"]["schema"], ref_keys));
                ref_keys.pop_back(); // pop body ref
            } else { // Otherwise validators without body
                per_path_validator.emplace(path, new ValidatorsStore());
            }

            if (method_itr->value.HasMember("parameters")) { //  if "method+path" has parameters
                ref_keys.emplace_back("parameters");
                per_path_validator.at(path)->AddParamValidators(path, method_itr->value["parameters"], ref_keys);
                ref_keys.pop_back();
            }

            if (std::string::npos != path.find('{') && std::string::npos != path.find('}')) { // has path params
                per_method_validator.path_trie.Insert(path);
            }
            ref_keys.pop_back(); // Pop the method key
        }
        ref_keys.pop_back(); // Pop the path key
    }
}

ValidationError OASValidatorImp::ValidateRoute(const std::string& method, const std::string& http_path, std::string& error_msg)
{
    ValidatorsStore* validators;
    return GetValidators(method, http_path, validators, error_msg);
}

ValidationError OASValidatorImp::ValidateBody(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg)
{
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateBody(json_body, error_msg);
}

ValidationError OASValidatorImp::ValidatePathParam(const std::string& method, const std::string& http_path, std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs);
    CHECK_ERROR(err_code)

    return validators->ValidatePathParams(param_idxs, error_msg);
}

ValidationError OASValidatorImp::ValidateQueryParam(const std::string& method, const std::string& http_path, std::string& error_msg)
{
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, nullptr, &query);
    CHECK_ERROR(err_code)

    return validators->ValidateQueryParams(query, error_msg);
}

ValidationError OASValidatorImp::ValidateHeaders(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers,
                                                 std::string& error_msg)
{
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateHeaderParams(headers, error_msg);
}

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path, std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs, &query);
    CHECK_ERROR(err_code)

    err_code = validators->ValidatePathParams(param_idxs, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateQueryParams(query, error_msg);
}

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs, &query);
    CHECK_ERROR(err_code)

    err_code = validators->ValidateBody(json_body, error_msg);
    CHECK_ERROR(err_code)

    err_code = validators->ValidatePathParams(param_idxs, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateQueryParams(query, error_msg);
}

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers,
                                                 std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs, &query);
    CHECK_ERROR(err_code)

    err_code = validators->ValidatePathParams(param_idxs, error_msg);
    CHECK_ERROR(err_code)

    err_code = validators->ValidateQueryParams(query, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateHeaderParams(headers, error_msg);
}

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body,
                                                 const std::unordered_map<std::string, std::string>& headers, std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs, &query);
    CHECK_ERROR(err_code)

    err_code = validators->ValidateBody(json_body, error_msg);
    CHECK_ERROR(err_code)

    err_code = validators->ValidatePathParams(param_idxs, error_msg);
    CHECK_ERROR(err_code)

    err_code = validators->ValidateQueryParams(query, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateHeaderParams(headers, error_msg);
}

OASValidatorImp::~OASValidatorImp()
{
#ifndef LUA_OAS_VALIDATOR // LUA manages garbage collection itself
    for (auto& per_method_validator : oas_validators_) {
        for (auto& per_path_validator : per_method_validator.per_path_validators) {
            delete per_path_validator.second;
        }
    }
#endif
}

ValidationError OASValidatorImp::GetValidators(const std::string& method, const std::string& http_path, ValidatorsStore*& validators, std::string& error_msg,
                                               std::unordered_map<size_t, ParamRange>* param_idxs, std::string* query)
{
    auto err_code = method_validator_.Validate(method, error_msg);
    CHECK_ERROR(err_code)

    auto enum_method = kStringToMethod.at(method);

    auto query_pos = http_path.find('?');
    if (std::string::npos != query_pos) {
        if (query) {
            *query = http_path.substr(query_pos + 1); // Extract everything after '?' as the query
        }
    }

    try {
        // 1st. try, no path params
        validators = oas_validators_[enum_method].per_path_validators.at(std::string::npos == query_pos ? http_path : http_path.substr(0, query_pos));
    } catch (const std::out_of_range&) {
        // 2nd try, if path has dynamic path parameters
        std::string map_key;
        map_key.reserve(http_path.length() + 32);
        const char* beg = http_path.c_str();
        const char* const end = http_path.c_str() + (std::string::npos == query_pos ? http_path.length() : query_pos);
        bool found = param_idxs ? oas_validators_[enum_method].path_trie.Search(beg, end, map_key, *param_idxs) : oas_validators_[enum_method].path_trie.Search(beg, end, map_key);
        if (found) {
            try {
                validators = oas_validators_[enum_method].per_path_validators.at(map_key);
            } catch (const std::out_of_range&) {
                error_msg = R"({"errorCode":"INVALID_ROUTE","detail":{"description": "Invalid HTTP method ')" + method + "' or path: '" + http_path + R"('"}})";
                return ValidationError::INVALID_ROUTE;
            }
        } else {
            error_msg = R"({"errorCode":"INVALID_ROUTE","detail":{"description": "Invalid HTTP method ')" + method + "' or path: '" + http_path + R"('"}})";
            return ValidationError::INVALID_ROUTE;
        }
    }
    return ValidationError::NONE;
}

const std::unordered_map<std::string, HttpMethod> OASValidatorImp::kStringToMethod = {{"GET", GET},         {"POST", POST},       {"PUT", PUT},         {"DELETE", DELETE},
                                                                                      {"HEAD", HEAD},       {"OPTIONS", OPTIONS}, {"PATCH", PATCH},     {"CONNECT", CONNECT},
                                                                                      {"TRACE", TRACE},     {"get", GET},         {"post", POST},       {"put", PUT},
                                                                                      {"delete", DELETE},   {"head", HEAD},       {"options", OPTIONS}, {"patch", PATCH},
                                                                                      {"connect", CONNECT}, {"trace", TRACE}};
