#include "validators/param_validators.hpp"
#include <gtest/gtest.h>

enum : size_t
{
    INPUT,
    EXPLODE,
    TYPE,
    EXPECTED_ERROR
};

class HeaderParamValidatorTest: public ::testing::TestWithParam<std::tuple<std::string, bool, std::string, ValidationError>>
{
protected:
    void SetUp() override
    {
        const auto param = GetParam();
        input_ = std::get<INPUT>(param);
        const auto* const explode = std::get<EXPLODE>(param) ? "true" : "false";
        const auto& type = std::get<TYPE>(param);
        expected_error_ = std::get<EXPECTED_ERROR>(param);
        std::string json = R"({
            "name": "param",
            "in": "header",
            "required": true,
            "style": "simple",
            "explode": )" + std::string(explode) +
                           R"(,
            "schema": {
                "type": ")" +
                           type + R"("
            }
        })";
        doc_.Parse(json.data());
        validator_ = std::make_unique<HeaderParamValidator>(doc_, keys_);
    }

    std::unique_ptr<HeaderParamValidator> validator_;
    std::string input_;
    rapidjson::Document doc_;
    std::vector<std::string> keys_{"paths", "/pets/xyz", "get", "parameters", "param"};
    std::string error_msg_;
    ValidationError expected_error_;
};

TEST_P(HeaderParamValidatorTest, ValidateParam)
{
    ASSERT_EQ(expected_error_, validator_->ValidateParam(input_.data(), input_.data() + input_.size(), error_msg_));
}

INSTANTIATE_TEST_CASE_P(HeaderParamValidatorTest, HeaderParamValidatorTest,
                        ::testing::Values(std::make_tuple("123", false, "integer", ValidationError::NONE), std::make_tuple("123", true, "integer", ValidationError::NONE),
                                          std::make_tuple("true", false, "boolean", ValidationError::NONE), std::make_tuple("true", true, "boolean", ValidationError::NONE),
                                          std::make_tuple("123.456", false, "number", ValidationError::NONE), std::make_tuple("123.456", true, "number", ValidationError::NONE),
                                          std::make_tuple("abc", false, "string", ValidationError::NONE), std::make_tuple("abc", true, "string", ValidationError::NONE)));
