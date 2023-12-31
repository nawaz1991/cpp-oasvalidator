/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef OAS_VALIDATORS_HPP
#define OAS_VALIDATORS_HPP

#include "utils/common.hpp"
#include "utils/path_trie.hpp"
#include "validators/body_validator.hpp"
#include "validators/param_validators.hpp"

#include <vector>
class ValidatorsStore
{
public:
    ValidatorsStore() = default;
    explicit ValidatorsStore(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys);
    void AddParamValidators(const std::string& path, const rapidjson::Value& params, std::vector<std::string>& ref_keys);
    ValidationError ValidateBody(const std::string& json_body, std::string& error_msg);
    ValidationError ValidatePathParams(std::unordered_map<size_t, ParamRange>& param_idxs, std::string& error_msg);
    ValidationError ValidateQueryParams(const std::string& query, std::string& error_msg);
    ValidationError ValidateHeaderParams(const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);
    ~ValidatorsStore();

private:
    struct PathParamValidatorInfo
    {
        int idx;
        PathParamValidator* validator;
    };

    struct QueryParamValidatorInfo
    {
        std::string name;
        QueryParamValidator* validator;
    };

    BodyValidator* body_validator_ = nullptr;
    std::vector<PathParamValidatorInfo> path_param_validators_;
    std::vector<QueryParamValidatorInfo> query_param_validators_;
    std::unordered_map<std::string, HeaderParamValidator*> header_param_validators_;

    static std::unordered_map<std::string, int> GetPathParamIndices(const std::string& path);
};

#endif // OAS_VALIDATORS_HPP
