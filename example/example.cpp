#include <oas_validator.hpp>

#include <iostream>
int main()
{
    OASValidator oasValidator("./openAPI_example.json");
    std::string err;
    if (oasValidator.ValidateRoute("GET1", "/test1", err) != ValidationError::NONE) // Invalid method
    {
        std::cout << err << std::endl;
    }
    if (oasValidator.ValidateRoute("GET", "/test1", err) != ValidationError::NONE) // Invalid path
    {
        std::cout << err << std::endl;
    }
    if (oasValidator.ValidateRequest("PUT", "/pet", R"({"name": "doggie", "photoUrls": "http://example.com/photo1.jpg"})",
                                     err) != ValidationError::NONE) // Invalid body
    {
        std::cout << err << std::endl;
    }
    if (oasValidator.ValidateRequest("PUT", "/pet", std::string(), err) != ValidationError::NONE) // Invalid body
    {
        std::cout << err << std::endl;
    }
    if (oasValidator.ValidatePathParam("GET", "/pet/1,40", err) != ValidationError::NONE) // Invalid path param
    {
        std::cout << err << std::endl;
    }
    std::string path = "/queryparam?stringParam=teststring&intParam=qwer&arrayParam1=string1&arrayParam1=string2&arrayParam2=1&arrayParam2=2&arrayParam2=3";
    if (oasValidator.ValidateQueryParam("PUT", path, err) != ValidationError::NONE) // Invalid Query
    {
        std::cout << err << std::endl;
    }
}