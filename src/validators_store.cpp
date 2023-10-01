/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "validators_store.hpp"

#include <set>

ValidatorsStore::ValidatorsStore(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys)
    : body_validator_(new BodyValidator(schema_val, ref_keys))
{
}

void ValidatorsStore::AddParamValidators(const std::string& path, const rapidjson::Value& params, std::vector<std::string>& ref_keys)
{
    auto path_param_idxs = GetPathParamIndices(path);
    for (const auto& param_val : params.GetArray()) {
        std::string in(param_val["in"].GetString());
        std::string name(param_val["name"].GetString());
        ref_keys.emplace_back(name);
        if ("path" == in) {
            path_param_validators_.emplace_back(PathParamValidatorInfo{path_param_idxs.at(name), new PathParamValidator(param_val, ref_keys)});
        } else if ("query" == in) {
            query_param_validators_.emplace_back(QueryParamValidatorInfo{name, new QueryParamValidator(param_val, ref_keys)});
        } else if ("header" == in) {
            header_param_validators_.emplace(name, new HeaderParamValidator(param_val, ref_keys));
        } else {
            // TODO: for cookies
        }
        ref_keys.pop_back();
    }
}

ValidationError ValidatorsStore::ValidateBody(const std::string& json_body, std::string& error_msg)
{
    if (body_validator_) {
        return body_validator_->Validate(json_body, error_msg);
    }
    return ValidationError::NONE; // No validator, no error
}

ValidationError ValidatorsStore::ValidatePathParams(std::unordered_map<size_t, ParamRange>& param_idxs, std::string& error_msg)
{
    for (auto& param_validator : path_param_validators_) {
        auto const& range = param_idxs.at(param_validator.idx);
        auto err_code = param_validator.validator->Validate(range.beg, range.end, error_msg);
        if (ValidationError::NONE != err_code) {
            return err_code;
        }
    }

    return ValidationError::NONE;
}

ValidationError ValidatorsStore::ValidateQueryParams(const std::string& query, std::string& error_msg)
{
    if (1 == query_param_validators_.size()) {
        return query_param_validators_.front().validator->Validate(query.data(), query.data() + query.length(), error_msg);
    }
    std::set<size_t> starts;
    std::unordered_map<std::string, size_t> start_map;
    for (const auto& param_validator : query_param_validators_) {
        auto start = query.find(param_validator.name);
        if (std::string::npos == start && param_validator.validator->IsRequired()) {
            return param_validator.validator->ErrorOnMissing(error_msg); // TODO for STYLE_FORM, TYPE_OBJECT, false
        } else {
            starts.emplace(start);
            start_map.emplace(param_validator.name, start);
        }
    }
    starts.emplace(query.length() + 1);

    for (auto& param_validator : query_param_validators_) {
        auto start = start_map.at(param_validator.name);
        auto end = (*std::next(starts.find(start))) - 1;
        auto err_code = param_validator.validator->Validate(query.data() + start, query.data() + end, error_msg);
        if (ValidationError::NONE != err_code) {
            return err_code;
        }
    }
    return ValidationError::NONE;
}

ValidationError ValidatorsStore::ValidateHeaderParams(const std::unordered_map<std::string, std::string>& headers, std::string& error_msg)
{
    for (auto& header_validator : header_param_validators_) {
        try {
            const auto& param = headers.at(header_validator.first);
            auto err_code = header_validator.second->Validate(param.data(), param.data() + param.length(), error_msg);
            if (ValidationError::NONE != err_code) {
                return err_code;
            }
        } catch (const std::out_of_range&) {
            if (header_validator.second->IsRequired()) {
                return header_validator.second->ErrorOnMissing(error_msg);
            }
        }
    }
    return ValidationError::NONE;
}
ValidatorsStore::~ValidatorsStore()
{
#ifndef LUA_OAS_VALIDATOR // LUA manages garbage collection itself
    for (auto& param_validator : query_param_validators_) {
        delete param_validator.validator;
    }
#endif
}

std::unordered_map<std::string, int> ValidatorsStore::GetPathParamIndices(const std::string& path)
{
    std::unordered_map<std::string, int> param_idxs;
    const char* dir_start = path.data();
    const char* const path_end = dir_start + path.length();
    const char* dir_end;
    int frag_idx = 0;

    while (dir_start < path_end) {
        dir_end = Seek(dir_start, path_end, '/');

        if (*dir_start == '{' && *(dir_end - 1) == '}') {
            param_idxs.emplace(std::string(dir_start + 1, dir_end - 1), frag_idx);
        }
        dir_start = dir_end + 1; // skip '/'
        frag_idx++;
    }
    return param_idxs;
}
