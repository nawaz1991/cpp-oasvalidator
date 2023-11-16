/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef OAS_VALIDATION_HPP
#define OAS_VALIDATION_HPP

#include "utils/common.hpp"
#include "utils/path_trie.hpp"
#include "validators/method_validator.hpp"
#include "validators/validators_store.hpp"

class OASValidatorImp
{
public:
    explicit OASValidatorImp(const std::string& specs_path);
    ValidationError ValidateRoute(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateBody(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg);
    ValidationError ValidatePathParam(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateQueryParam(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateHeaders(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body,
                                    const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);
    ~OASValidatorImp();

private:
    static const std::unordered_map<std::string, HttpMethod> kStringToMethod;

    struct PerMethod
    {
        std::unordered_map<std::string, ValidatorsStore*> per_path_validators;
        PathTrie path_trie;
    };

    std::array<PerMethod, static_cast<size_t>(HttpMethod::COUNT)> oas_validators_;
    MethodValidator method_validator_;

    ValidationError GetValidators(const std::string& method, const std::string& http_path, ValidatorsStore*& validators, std::string& error_msg,
                                  std::unordered_map<size_t, ParamRange>* param_idxs = nullptr, std::string* query = nullptr);
};

#endif // OAS_VALIDATION_HPP
