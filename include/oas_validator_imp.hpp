/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef OAS_VALIDATION_HPP
#define OAS_VALIDATION_HPP

#include "utils/common.hpp"
#include "utils/path_trie.hpp"
#include "validators/method_validator.hpp"
#include "validators/validators_store.hpp"

class OASValidatorImp
{
public:
    explicit OASValidatorImp(const std::string& oas_specs,
                             const std::unordered_map<std::string, std::unordered_set<std::string>>& method_map = {});
    ValidationError ValidateRoute(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateBody(const std::string& method, const std::string& http_path, const std::string& json_body,
                                 std::string& error_msg);
    ValidationError ValidatePathParam(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateQueryParam(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateHeaders(const std::string& method, const std::string& http_path,
                                    const std::unordered_map<std::string, std::string>& headers,
                                    std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path,
                                    const std::string& json_body, std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path,
                                    const std::unordered_map<std::string, std::string>& headers,
                                    std::string& error_msg);
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path,
                                    const std::string& json_body,
                                    const std::unordered_map<std::string, std::string>& headers,
                                    std::string& error_msg);
    ~OASValidatorImp();

private:
    static const std::unordered_map<std::string, HttpMethod> kStringToMethod;

    struct PerMethod
    {
        std::unordered_map<std::string, ValidatorsStore*> per_path_validators{};
        PathTrie path_trie{};
    };

    const std::unordered_map<std::string, std::unordered_set<std::string>> method_map_;
    std::array<PerMethod, static_cast<size_t>(HttpMethod::COUNT)> oas_validators_{};
    MethodValidator method_validator_{};

    ValidationError GetValidators(const std::string& method, const std::string& mapped_method,
                                  const std::string& http_path, ValidatorsStore*& validators, std::string& error_msg,
                                  std::unordered_map<size_t, ParamRange>* param_idxs = nullptr,
                                  std::string* query = nullptr);
    ValidationError GetValidators(const std::string& method, const std::string& http_path, ValidatorsStore*& validators,
                                  std::string& error_msg, std::unordered_map<size_t, ParamRange>* param_idxs = nullptr,
                                  std::string* query = nullptr);
    static std::vector<std::string> Split(const std::string& str);
    static rapidjson::Value* ResolvePath(rapidjson::Document& doc, const std::string& path);
    static void ParseSpecs(const std::string& oas_specs, rapidjson::Document& doc);
    void ProcessPath(const rapidjson::Value::ConstMemberIterator& path_itr, std::vector<std::string>& ref_keys);
    void ProcessMethod(const rapidjson::Value::ConstMemberIterator& method_itr, const std::string& path,
                       std::vector<std::string>& ref_keys);
    static void ProcessRequestBody(const rapidjson::Value::ConstMemberIterator& method_itr, const std::string& path,
                                   std::vector<std::string>& ref_keys,
                                   std::unordered_map<std::string, ValidatorsStore*>& per_path_validator);
    static void ProcessParameters(const rapidjson::Value::ConstMemberIterator& method_itr, const std::string& path,
                                  std::vector<std::string>& ref_keys,
                                  std::unordered_map<std::string, ValidatorsStore*>& per_path_validator);
    void ResolveReferences(rapidjson::Value& value, rapidjson::Document& doc,
                           rapidjson::Document::AllocatorType& allocator);
    static std::unordered_map<std::string, std::unordered_set<std::string>>
    BuildCaseInsensitiveMap(const std::unordered_map<std::string, std::unordered_set<std::string>>& method_map);
};

#endif // OAS_VALIDATION_HPP
