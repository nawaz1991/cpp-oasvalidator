#include "validators/param_validators.hpp"
#include <gtest/gtest.h>

enum : size_t
{
    INPUT,
    STYLE,
    EXPLODE,
    TYPE,
    EXPECTED_ERROR
};

class QueryPrimitiveParam: public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, std::string, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/xyz?" + std::get<INPUT>(params);
        std::string style = std::get<STYLE>(params);
        bool explode = std::get<EXPLODE>(params);
        std::string type = std::get<TYPE>(params);
        expected_error_ = std::get<EXPECTED_ERROR>(params);

        std::string json = R"({
            "name": "param",
            "in": "query",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": ")" +
                           type + R"("
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<QueryParamValidator>(doc_, keys_);
    }

    std::unique_ptr<QueryParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/xyz", "get", "parameters", "param"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(QueryPrimitiveParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/xyz?").size();
    const char* end = input_.c_str() + input_.size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_CASE_P(QueryPrimitiveParamTests, QueryPrimitiveParam,
                        ::testing::Values(std::make_tuple("param=123", "form", false, "integer", ValidationError::NONE),
                                          std::make_tuple("param=123", "form", true, "integer", ValidationError::NONE),
                                          std::make_tuple("param=123.123", "form", false, "number", ValidationError::NONE),
                                          std::make_tuple("param=123.123", "form", true, "number", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz", "form", false, "string", ValidationError::NONE),
                                          std::make_tuple("param=abc", "form", true, "string", ValidationError::NONE),
                                          std::make_tuple("param=true", "form", false, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=true", "form", true, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=string", "form", false, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=string", "form", true, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=123.123", "form", false, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=123.123", "form", true, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=true", "form", false, "number", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=true", "form", true, "number", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=123", "form", false, "boolean", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=123", "form", true, "boolean", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=123.123", "form", false, "boolean", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=abc%2yz", "form", true, "string", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=abc%2yz", "form", false, "string", ValidationError::INVALID_QUERY_PARAM)));

class QueryArrayParam: public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, std::string, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/xyz?" + std::get<INPUT>(params);
        std::string style = std::get<STYLE>(params);
        bool explode = std::get<EXPLODE>(params);
        std::string type = std::get<TYPE>(params);
        expected_error_ = std::get<EXPECTED_ERROR>(params);

        std::string json = R"({
            "name": "param",
            "in": "query",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": "array",
                "items": {
                    "type": ")" +
                           type + R"("
                }
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<QueryParamValidator>(doc_, keys_);
    }

    std::unique_ptr<QueryParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/xyz", "get", "parameters", "param"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(QueryArrayParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/xyz?").size();
    const char* end = input_.c_str() + input_.size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_CASE_P(QueryArrayParamTests, QueryArrayParam,
                        ::testing::Values(std::make_tuple("param=3,4,5", "form", false, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3&param=4&param=5", "form", true, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3.3,4.4,5.5", "form", false, "number", ValidationError::NONE),
                                          std::make_tuple("param=3.3&param=4.4&param=5.5", "form", true, "number", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz,def%20xyz,ghi%20xyz", "form", false, "string", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz&param=def%20xyz&param=ghi%20xyz", "form", true, "string", ValidationError::NONE),
                                          std::make_tuple("param=true,false,true", "form", false, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=true&param=false&param=true", "form", true, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=string,abc,xyz", "form", false, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=string&param=abc&param=xyz", "form", true, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=3.3,4.4,5.5", "form", false, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=3.3&param=4.4&param=5.5", "form", true, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=true,false,true", "form", false, "number", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=true&param=false&param=true", "form", true, "number", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=3,4,5", "form", false, "boolean", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=3%204%205", "spaceDelimited", false, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3&param=4&param=5", "spaceDelimited", true, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3.3%204.4%205.5", "spaceDelimited", false, "number", ValidationError::NONE),
                                          std::make_tuple("param=3.3&param=4.4&param=5.5", "spaceDelimited", true, "number", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz%20def%20xyz%20ghi%20xyz", "spaceDelimited", false, "string", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz&param=def%20xyz&param=ghi%20xyz", "spaceDelimited", true, "string", ValidationError::NONE),
                                          std::make_tuple("param=true%20false%20true", "spaceDelimited", false, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=true&param=false&param=true", "spaceDelimited", true, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=string%20abc%20xyz", "spaceDelimited", false, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=string&param=abc&param=xyz", "spaceDelimited", true, "integer", ValidationError::INVALID_QUERY_PARAM),
                                          std::make_tuple("param=3|4|5", "pipeDelimited", false, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3&param=4&param=5", "pipeDelimited", true, "integer", ValidationError::NONE),
                                          std::make_tuple("param=3.3|4.4|5.5", "pipeDelimited", false, "number", ValidationError::NONE),
                                          std::make_tuple("param=3.3&param=4.4&param=5.5", "pipeDelimited", true, "number", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz|def%20xyz|ghi%20xyz", "pipeDelimited", false, "string", ValidationError::NONE),
                                          std::make_tuple("param=abc%20xyz&param=def%20xyz&param=ghi%20xyz", "pipeDelimited", true, "string", ValidationError::NONE),
                                          std::make_tuple("param=true|false|true", "pipeDelimited", false, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=true&param=false&param=true", "pipeDelimited", true, "boolean", ValidationError::NONE),
                                          std::make_tuple("param=string|abc|xyz", "pipeDelimited", false, "integer", ValidationError::INVALID_QUERY_PARAM)));

class QueryObjectParam: public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, ValidationError>>
{
protected:
    void SetUp() override
    {
        auto params = GetParam();
        input_ = "/pets/xyz?" + std::get<0>(params);
        std::string style = std::get<1>(params);
        bool explode = std::get<2>(params);
        expected_error_ = std::get<3>(params);

        std::string json = R"({
            "name": "param",
            "in": "query",
            "required": true,
            "style": ")" + style +
                           R"(",
            "explode": )" + (explode ? "true" : "false") +
                           R"(,
            "schema": {
                "type": "object",
                "properties": {
                    "boolTrue": {
                        "type": "boolean"
                    },
                    "boolFalse": {
                        "type": "boolean"
                    },
                    "int": {
                        "type": "integer"
                    },
                    "number": {
                        "type": "number"
                    },
                    "string": {
                        "type": "string"
                    }
                }
            }
        })";
        doc_.Parse(json.c_str());

        validator_ = std::make_unique<QueryParamValidator>(doc_, keys_);
    }

    std::unique_ptr<QueryParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/xyz", "get", "parameters", "param"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(QueryObjectParam, ValidateParam)
{
    const char* beg = input_.c_str() + std::string("/pets/xyz?").size();
    const char* end = input_.c_str() + input_.size();
    auto result = validator_->ValidateParam(beg, end, error_msg_);
    EXPECT_EQ(result, expected_error_);
    if (result != ValidationError::NONE) {
        EXPECT_FALSE(error_msg_.empty());
        rapidjson::Document doc;
        doc.Parse(error_msg_.c_str());
        EXPECT_FALSE(doc.HasParseError());
    }
}

INSTANTIATE_TEST_CASE_P(QueryObjectParamTests, QueryObjectParam,
                        ::testing::Values(std::make_tuple("param=boolTrue,true,boolFalse,false,int,123,number,123.456,string,abc%20xyz", "form", false, ValidationError::NONE),
                                          std::make_tuple("boolTrue=true&boolFalse=false&int=123&number=123.456&string=abc%20xyz", "form", true, ValidationError::NONE),
                                          std::make_tuple("param[boolTrue]=true&param[boolFalse]=false&param[int]=123&param[number]=123.456&param[string]=abc%20xyz", "deepObject",
                                                          true, ValidationError::NONE)));