#include <oas_validator.hpp>

#include <iostream>
#include <vector>

int main()
{
    OASValidator oas_validator(SPEC_PATH);
    std::string err_msg;

    if (ValidationError::NONE != oas_validator.ValidateRoute("GETT", "/test/dummy", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidateRoute("GET", "/test/dummy", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidateRoute("GET", "/test/invalid", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidatePathParam("GET", "/test/integer_simple_true/1001", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidatePathParam("GET", "/test/integer_simple_true/not_an_integer", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidateQueryParam("GET", "/test/query_integer_form_true?param=not_an_integer", err_msg)) {
        std::cout << err_msg << std::endl;
    }

    std::unordered_map<std::string, std::string> headers;
    headers["intHeader"] = "not_an_integer";
    if (ValidationError::NONE != oas_validator.ValidateHeaders("GET", "/test/header_single1", headers, err_msg)) {
        std::cout << err_msg << std::endl;
    }

    if (ValidationError::NONE != oas_validator.ValidateRequest("POST",
                                                               "/test/all/123/abc/str1,str2/"
                                                               "field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&"
                                                               "param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
                                                               "{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":"
                                                               "\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":true,\"field7\":[123,456],\"field8\":[123,456],\"field9\":"
                                                               "\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}",
                                                               headers, err_msg)) {
        std::cout << err_msg << std::endl;
    }
}