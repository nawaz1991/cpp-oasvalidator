#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include "common.hpp"

#include <string>
#include <unordered_map>

class BaseValidator
{
public:
    explicit BaseValidator(ValidationError err_code);
    explicit BaseValidator(const std::vector<std::string>& ref_keys, ValidationError err_code);

    virtual ValidationError Validate(const std::string& content, std::string& err_msg) = 0;
    virtual ~BaseValidator() = default;

protected:
    ValidationError code_on_error_;
    std::string err_header_;

    void InitErrHeader();

private:
    static const std::unordered_map<ValidationError, std::string> kErrHeaders;
};

#endif // VALIDATOR_HPP
