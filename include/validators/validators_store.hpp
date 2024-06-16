/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef OAS_VALIDATORS_HPP
#define OAS_VALIDATORS_HPP

#include "utils/common.hpp"
#include "utils/path_trie.hpp"
#include "validators/body_validator.hpp"
#include "validators/param_validators.hpp"

#include <utility>
#include <vector>
class ValidatorsStore
{
public:
    ValidatorsStore() = default;
    explicit ValidatorsStore(const rapidjson::Value& schema_val, const std::vector<std::string>& ref_keys);
    ValidatorsStore(const ValidatorsStore&) = delete;
    ValidatorsStore& operator=(const ValidatorsStore&) = delete;
    void AddParamValidators(const std::string& path, const rapidjson::Value& params,
                            std::vector<std::string>& ref_keys);
    ValidationError ValidateBody(const std::string& json_body, std::string& error_msg);
    ValidationError ValidatePathParams(std::unordered_map<size_t, ParamRange>& param_idxs, std::string& error_msg);
    ValidationError ValidateQueryParams(const std::string& query, std::string& error_msg);
    ValidationError ValidateHeaderParams(const std::unordered_map<std::string, std::string>& headers,
                                         std::string& error_msg);
    ~ValidatorsStore();

private:
    struct PathParamValidatorInfo
    {
        PathParamValidatorInfo(size_t idx, PathParamValidator* validator)
            : idx(idx)
            , validator(validator)
        {
        }

        PathParamValidatorInfo(const PathParamValidatorInfo& other) = default;

        PathParamValidatorInfo& operator=(const PathParamValidatorInfo& other)
        {
            if (this == &other) {
                return *this;
            }
            idx = other.idx;
            validator = other.validator;
            return *this;
        }

        size_t idx = std::string::npos;
        PathParamValidator* validator = nullptr;
    };

    struct QueryParamValidatorInfo
    {
        QueryParamValidatorInfo(std::string name, QueryParamValidator* validator)
            : name(std::move(name))
            , validator(validator)
        {
        }

        QueryParamValidatorInfo(const QueryParamValidatorInfo& other) = default;

        QueryParamValidatorInfo& operator=(const QueryParamValidatorInfo& other)
        {
            if (this == &other) {
                return *this;
            }
            name = other.name;
            validator = other.validator;
            return *this;
        }

        std::string name{};
        QueryParamValidator* validator = nullptr;

        ~QueryParamValidatorInfo()
        {
        }
    };

    BodyValidator* body_validator_ = nullptr;
    std::vector<PathParamValidatorInfo> path_param_validators_{};
    std::vector<QueryParamValidatorInfo> query_param_validators_{};
    std::unordered_map<std::string, HeaderParamValidator*> header_param_validators_{};

    static std::unordered_map<std::string, size_t> GetPathParamIndices(const std::string& path);
};

#endif // OAS_VALIDATORS_HPP
