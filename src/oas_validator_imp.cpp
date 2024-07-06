/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "oas_validator_imp.hpp"
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <sstream>

OASValidatorImp::OASValidatorImp(const std::string& oas_specs,
                                 const std::unordered_map<std::string, std::unordered_set<std::string>>& method_map)
    : method_map_(method_map)
{
    rapidjson::Document doc;
    ParseSpecs(oas_specs, doc);
    ResolveReferences(doc, doc, doc.GetAllocator());

    const rapidjson::Value& paths = doc["paths"];
    std::vector<std::string> ref_keys;
    ref_keys.emplace_back("paths");

    for (auto path_itr = paths.MemberBegin(); path_itr != paths.MemberEnd(); ++path_itr) {
        ProcessPath(path_itr, ref_keys);
    }
}

ValidationError OASValidatorImp::ValidateRoute(const std::string& method, const std::string& http_path,
                                               std::string& error_msg)
{
    ValidatorsStore* validators;
    return GetValidators(method, http_path, validators, error_msg);
}

ValidationError OASValidatorImp::ValidateBody(const std::string& method, const std::string& http_path,
                                              const std::string& json_body, std::string& error_msg)
{
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateBody(json_body, error_msg);
}

ValidationError OASValidatorImp::ValidatePathParam(const std::string& method, const std::string& http_path,
                                                   std::string& error_msg)
{
    std::unordered_map<size_t, ParamRange> param_idxs;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, &param_idxs);
    CHECK_ERROR(err_code)

    return validators->ValidatePathParams(param_idxs, error_msg);
}

ValidationError OASValidatorImp::ValidateQueryParam(const std::string& method, const std::string& http_path,
                                                    std::string& error_msg)
{
    std::string query;
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg, nullptr, &query);
    CHECK_ERROR(err_code)

    return validators->ValidateQueryParams(query, error_msg);
}

ValidationError OASValidatorImp::ValidateHeaders(const std::string& method, const std::string& http_path,
                                                 const std::unordered_map<std::string, std::string>& headers,
                                                 std::string& error_msg)
{
    ValidatorsStore* validators;

    auto err_code = GetValidators(method, http_path, validators, error_msg);
    CHECK_ERROR(err_code)

    return validators->ValidateHeaderParams(headers, error_msg);
}

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path,
                                                 std::string& error_msg)
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

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path,
                                                 const std::string& json_body, std::string& error_msg)
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

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path,
                                                 const std::unordered_map<std::string, std::string>& headers,
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

ValidationError OASValidatorImp::ValidateRequest(const std::string& method, const std::string& http_path,
                                                 const std::string& json_body,
                                                 const std::unordered_map<std::string, std::string>& headers,
                                                 std::string& error_msg)
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

ValidationError OASValidatorImp::GetValidators(const std::string& method, const std::string& http_path,
                                               ValidatorsStore*& validators, std::string& error_msg,
                                               std::unordered_map<size_t, ParamRange>* param_idxs, std::string* query)
{
    auto err_code = method_validator_.Validate(method, error_msg);
    CHECK_ERROR(err_code)

    err_code = GetValidators(method, method, http_path, validators, error_msg, param_idxs, query);
    if (ValidationError::INVALID_ROUTE == err_code) {
        try {
            auto mapped_methods = method_map_.at(method);
            for (const auto& mapped_method : mapped_methods) {
                err_code = GetValidators(method, mapped_method, http_path, validators, error_msg, param_idxs, query);
                if (ValidationError::NONE == err_code) {
                    return err_code;
                }
            }
        } catch (const std::out_of_range&) {
            return err_code;
        }
    }

    return err_code;
}

ValidationError OASValidatorImp::GetValidators(const std::string& method, const std::string& mapped_method,
                                               const std::string& http_path, ValidatorsStore*& validators,
                                               std::string& error_msg,
                                               std::unordered_map<size_t, ParamRange>* param_idxs, std::string* query)
{
    auto enum_method = kStringToMethod.at(mapped_method);

    auto query_pos = http_path.find('?');
    if (std::string::npos != query_pos && query) {
        *query = http_path.substr(query_pos);
    }

    try {
        // 1st. try, no path params
        validators = oas_validators_[static_cast<size_t>(enum_method)].per_path_validators.at(
            std::string::npos == query_pos ? http_path : http_path.substr(0, query_pos));
    } catch (const std::out_of_range&) {
        // 2nd try, if path has dynamic path parameters
        std::string map_key;
        map_key.reserve(http_path.length() + 32);
        const char* beg = http_path.c_str();
        const char* const end = http_path.c_str() + (std::string::npos == query_pos ? http_path.length() : query_pos);
        bool found = param_idxs ? oas_validators_[static_cast<size_t>(enum_method)].path_trie.Search(beg, end, map_key,
                                                                                                     *param_idxs)
                                : oas_validators_[static_cast<size_t>(enum_method)].path_trie.Search(beg, end, map_key);
        if (found) {
            try {
                validators = oas_validators_[static_cast<size_t>(enum_method)].per_path_validators.at(map_key);
            } catch (const std::out_of_range&) {
                error_msg = R"({"errorCode":"INVALID_ROUTE","details":{"description": "Invalid HTTP method ')" +
                            method + "' or path: '" + http_path + R"('"}})";
                return ValidationError::INVALID_ROUTE;
            }
        } else {
            error_msg = R"({"errorCode":"INVALID_ROUTE","details":{"description": "Invalid HTTP method ')" + method +
                        "' or path: '" + http_path + R"('"}})";
            return ValidationError::INVALID_ROUTE;
        }
    }
    return ValidationError::NONE;
}

std::vector<std::string> OASValidatorImp::Split(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (getline(token_stream, token, '/')) {
        tokens.push_back(token);
    }
    return tokens;
}

rapidjson::Value* OASValidatorImp::ResolvePath(rapidjson::Document& doc, const std::string& path)
{
    std::vector<std::string> parts = Split(path);
    rapidjson::Value* current = &doc;
    for (const std::string& part : parts) {
        if (!current->IsObject() || !current->HasMember(part.c_str())) {
            throw ValidatorInitExc("Invalid path or missing member:" + part);
        }
        current = &(*current)[part.c_str()];
    }
    return current;
}

void OASValidatorImp::ParseSpecs(const std::string& oas_specs, rapidjson::Document& doc)
{
    std::ifstream ifs(oas_specs);

    if (ifs.is_open()) {
        rapidjson::IStreamWrapper isw(ifs);
        doc.ParseStream(isw);
        ifs.close();
    } else {
        doc.Parse(oas_specs.c_str());
    }

    if (doc.HasParseError()) {
        throw ValidatorInitExc("Unable to parse specs: " + oas_specs +
                               " \nError code: " + std::to_string(doc.GetParseError()) +
                               " at offset: " + std::to_string(doc.GetErrorOffset()) +
                               " Error message: " + rapidjson::GetParseError_En(doc.GetParseError()));
    }
}

void OASValidatorImp::ProcessPath(const rapidjson::Value::ConstMemberIterator& path_itr,
                                  std::vector<std::string>& ref_keys)
{
    std::string path(path_itr->name.GetString());
    ref_keys.emplace_back(EscapeSlash(path));
    const rapidjson::Value& methods = path_itr->value;

    for (auto method_itr = methods.MemberBegin(); method_itr != methods.MemberEnd(); ++method_itr) {
        ProcessMethod(method_itr, path, ref_keys);
    }

    ref_keys.pop_back(); // Pop the path key
}

void OASValidatorImp::ProcessMethod(const rapidjson::Value::ConstMemberIterator& method_itr, const std::string& path,
                                    std::vector<std::string>& ref_keys)
{
    ref_keys.emplace_back(method_itr->name.GetString());
    auto method(kStringToMethod.at(method_itr->name.GetString()));
    auto& per_method_validator = oas_validators_[static_cast<size_t>(method)];
    auto& per_path_validator = per_method_validator.per_path_validators;

    ProcessRequestBody(method_itr, path, ref_keys, per_path_validator);
    ProcessParameters(method_itr, path, ref_keys, per_path_validator);

    if (std::string::npos != path.find('{') && std::string::npos != path.find('}')) { // has path params
        per_method_validator.path_trie.Insert(path);
    }

    ref_keys.pop_back(); // Pop the method key
}

void OASValidatorImp::ProcessRequestBody(const rapidjson::Value::ConstMemberIterator& method_itr,
                                         const std::string& path, std::vector<std::string>& ref_keys,
                                         std::unordered_map<std::string, ValidatorsStore*>& per_path_validator)
{
    if ((method_itr->value.HasMember("requestBody")) && (method_itr->value["requestBody"].HasMember("content")) &&
        (method_itr->value["requestBody"]["content"].HasMember("application/json")) &&
        (method_itr->value["requestBody"]["content"]["application/json"].HasMember(
            "schema"))) { //  if "method+path" has json body
        ref_keys.emplace_back("requestBody/content/application%2Fjson/schema");
        per_path_validator.emplace(
            path,
            new ValidatorsStore(method_itr->value["requestBody"]["content"]["application/json"]["schema"], ref_keys));
        ref_keys.pop_back(); // pop body ref
    } else { // Otherwise validators without body
        per_path_validator.emplace(path, new ValidatorsStore());
    }
}

void OASValidatorImp::ProcessParameters(const rapidjson::Value::ConstMemberIterator& method_itr,
                                        const std::string& path, std::vector<std::string>& ref_keys,
                                        std::unordered_map<std::string, ValidatorsStore*>& per_path_validator)
{
    if (method_itr->value.HasMember("parameters")) { //  if "method+path" has parameters
        ref_keys.emplace_back("parameters");
        per_path_validator.at(path)->AddParamValidators(path, method_itr->value["parameters"], ref_keys);
        ref_keys.pop_back();
    }
}

void OASValidatorImp::ResolveReferences(rapidjson::Value& value, rapidjson::Document& doc,
                                        rapidjson::Document::AllocatorType& allocator)
{
    if (value.IsObject()) {
        bool should_reiterate;
        do {
            should_reiterate = false;
            for (auto itr = value.MemberBegin(); itr != value.MemberEnd();) {
                if (strcmp(itr->name.GetString(), "$ref") == 0 && itr->value.IsString()) {
                    std::string ref = itr->value.GetString();
                    if (ref.rfind("#/", 0) == 0) {
                        ref.erase(0, 2); // Remove '#/'
                        rapidjson::Value const* ref_value = ResolvePath(doc, ref);
                        if (ref_value) {
                            value.CopyFrom(*ref_value, allocator);
                            should_reiterate = true; // Signal to reiterate due to structural changes
                            break; // Exit the loop to avoid invalid iterator use
                        }
                    }
                } else {
                    ResolveReferences(itr->value, doc, allocator);
                }
                ++itr;
            }
        } while (should_reiterate); // Keep resolving until no more references are found
    } else if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
            ResolveReferences(value[i], doc, allocator);
        }
    }
}

const std::unordered_map<std::string, HttpMethod> OASValidatorImp::kStringToMethod = {
    {"GET", HttpMethod::GET},       {"POST", HttpMethod::POST},       {"PUT", HttpMethod::PUT},
    {"DELETE", HttpMethod::DELETE}, {"HEAD", HttpMethod::HEAD},       {"OPTIONS", HttpMethod::OPTIONS},
    {"PATCH", HttpMethod::PATCH},   {"CONNECT", HttpMethod::CONNECT}, {"TRACE", HttpMethod::TRACE},
    {"get", HttpMethod::GET},       {"post", HttpMethod::POST},       {"put", HttpMethod::PUT},
    {"delete", HttpMethod::DELETE}, {"head", HttpMethod::HEAD},       {"options", HttpMethod::OPTIONS},
    {"patch", HttpMethod::PATCH},   {"connect", HttpMethod::CONNECT}, {"trace", HttpMethod::TRACE}};
