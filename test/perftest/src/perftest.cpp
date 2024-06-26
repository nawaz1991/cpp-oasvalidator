/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "oas_validator.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <string>
#include <unordered_map>

class OASValidatorPerf: public ::benchmark::Fixture
{
public:
    void SetUp(benchmark::State& /*unused*/) override
    {
        validator = std::make_unique<OASValidator>(SPEC_PATH);
    }

    OASValidatorPerf()
        : ::benchmark::Fixture()
        , validator(nullptr)

    {
        constexpr static double K_MIN_WARM_UP_TIME = 5.0;
        constexpr static double K_MIN_TIME = 3.50;
        SetDefaultTimeUnit(::benchmark::kMicrosecond);
        MinWarmUpTime(K_MIN_WARM_UP_TIME);
        MinTime(K_MIN_TIME);
    }

    void TearDown(benchmark::State& /*unused*/) override
    {
        validator.reset();
    }

    std::unique_ptr<OASValidator> validator;
};

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidMethod /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateRoute("GETT", "/test/dummy", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidRoute /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateRoute("GET", "/test/dummy", err_msg);
    }
}
BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRoute /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateRoute("GET", "/test/invalid", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidPathParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidatePathParam("GET", "/test/integer_simple_true/123", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidPathParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidatePathParam("GET", "/test/integer_simple_true/not_an_integer", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidQueryParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateQueryParam("GET", "/test/query_integer_form_true?param=123", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidQueryParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateQueryParam("GET", "/test/query_integer_form_true?param=not_an_integer", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidHeaderParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["intHeader"] = "123";
    for (auto _ : state) {
        validator->ValidateHeaders("GET", "/test/header_single1", headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidHeaderParam /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["intHeader"] = "not_an_integer";
    for (auto _ : state) {
        validator->ValidateHeaders("GET", "/test/header_single1", headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidBody /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateBody("POST", "/test/body_scenario20", R"({"level1":{"level2":{"level3":"abc"}}})", err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidBody /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    for (auto _ : state) {
        validator->ValidateBody("POST", "/test/body_scenario20", R"({"level1":{"level2":{"not_a_level":"abc"}}})",
                                err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ValidRequest /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST", "/test/all/123?param4=123&param4=456",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRequest1 /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp) // Check 1: Invalid method
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POSTT", "/test/all/123?param4=123&param4=456",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRequest2 /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp) // Check 2: Invalid Path
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST", "/invalid/path",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}
BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRequest3 /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp) // Check 3: Invalid Path param
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST", "/test/all/not_a_number?param4=123&param4=456",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRequest4 /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp) // Check 4: Invalid Query param
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST", "/test/all/123?param4=not_a_number&param4=456",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, InvalidRequest5 /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp) // Check 5: Invalid Header param
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "not_a_bool";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST", "/test/all/123?param4=123&param4=456",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"}})",
            headers, err_msg);
    }
}

BENCHMARK_F(OASValidatorPerf /*unused*/, ComplexValidRequest /*unused*/)(benchmark::State& state) // NOLINT(cert-err58-cpp)
{
    std::string err_msg;
    std::unordered_map<std::string, std::string> headers;
    headers["param11"] = "true";
    headers["param12"] = "field1=0&field2=string";
    for (auto _ : state) {
        validator->ValidateRequest(
            "POST",
            "/test/all/123/abc/str1,str2/"
            "field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,"
            "string&"
            "param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
            R"({"field1":123,"field2":"abc","field3":["abc","def"],"field4":{"subfield1":123,"subfield2":"abc"},"field5":{"subfield1":123},"field6":"abc",)"
            R"("field7":[123,456],"field8":[123,456],"field9":"abc","field10":"option1","field11":{"field":123},"field12":[{"name":"abc"},{"name":"def"}]})",
            headers, err_msg);
    }
}
BENCHMARK_MAIN(); // NOLINT(cert-err58-cpp)
