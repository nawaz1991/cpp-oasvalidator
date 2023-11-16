/**
 * @file oas_validator.hpp
 * @author Muhammad Nawaz
 * @date 2023
 * @brief Header file for the OASValidator library.
 *
 * This file provides API for validating HTTP requests against OAS (OpenAPI Specification).
 *
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef OAS_VALIDATOR_HPP
#define OAS_VALIDATOR_HPP

#include <exception>
#include <string>
#include <unordered_map>

class ValidatorInitExc; ///< Forward declaration for the custom exception class.
class OASValidatorImp; ///< Forward declaration for the implementation class.

/**
 * @brief Enum class for specifying validation errors.
 *
 * This enum class is used to categorize different types of validation errors that might
 * occur during the validation process.
 */
#ifndef VALIDATION_ERROR
#define VALIDATION_ERROR
enum class ValidationError
{
    NONE = 0, ///< No error occurred.
    INVALID_METHOD = -1, ///< The HTTP method is not valid.
    INVALID_ROUTE = -2, ///< The HTTP route is not valid.
    INVALID_PATH_PARAM = -3, ///< The path parameter is not valid.
    INVALID_QUERY_PARAM = -4, ///< The query parameter is not valid.
    INVALID_HEADER_PARAM = -5, ///< The header parameter is not valid.
    INVALID_BODY = -6, ///< The request body is not valid.
    INVALID_RSP = -7 ///< The response is not valid.
};
#endif

/**
 * @brief Class that provides API for HTTP requests validation against OAS validation.
 *
 * The OASValidator class offers various methods for validating REST requests
 * against a defined OAS (OpenAPI Specification) file.
 */
class OASValidator
{
private:
    OASValidatorImp* impl_; ///< Pointer to the implementation object.

public:
    /**
     * @brief Constructor that takes the path to the OAS specification file.
     * @param spec_path File path to the OAS specification in JSON format.
     */
    explicit OASValidator(const std::string& spec_path);

    /**
     * @brief Validates the HTTP method and route against the OpenAPI specification.
     *
     * This function performs the following validation sequence:
     * 1. HTTP method
     * 2. Route
     *
     * @param method The HTTP method as a std::string (e.g., "GET", "POST").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */

    ValidationError ValidateRoute(const std::string& method, const std::string& http_path, std::string& error_msg);
    /**
     * @brief Validates the JSON body of the HTTP request against the OpenAPI specification.
     *
     * This function performs the following validation sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Body schema
     *
     * @param method The HTTP method as a std::string (e.g., "POST", "PUT").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param json_body The JSON body of the HTTP request as a std::string.
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_BODY: Invalid request body.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateBody(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg);

    /**
     * @brief Validates the path parameters of the HTTP request against the OpenAPI specification.
     *
     * This function performs the following validation sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Path parameters (in the sequence provided in the OpenAPI spec)
     *
     * @param method The HTTP method as a std::string (e.g., "GET", "DELETE").
     * @param http_path The HTTP path with parameters as a std::string (e.g., "/api/v1/resource/{id}").
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_PATH_PARAM: Invalid path parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidatePathParam(const std::string& method, const std::string& http_path, std::string& error_msg);

    /**
     * @brief Validates the query parameters of the HTTP request against the OpenAPI specification.
     *
     * This function performs the following validation sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Query parameters (in the sequence provided in the OpenAPI spec)
     *
     * @param method The HTTP method as a std::string (e.g., "GET", "DELETE").
     * @param http_path The HTTP path including query parameters as a std::string (e.g., "/api/v1/resource?name=value").
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_QUERY_PARAM: Invalid query parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateQueryParam(const std::string& method, const std::string& http_path, std::string& error_msg);

    /**
     * @brief Validates the HTTP headers of the request against the OpenAPI specification.
     *
     * This function performs the following validation sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Header parameters
     *
     * @param method The HTTP method as a std::string (e.g., "GET", "POST").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param headers The HTTP headers as an std::unordered_map from std::string to std::string.
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible return values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_HEADER_PARAM: Invalid header parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */

    ValidationError ValidateHeaders(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);

    /**
     * @brief Validates the entire HTTP request against the OpenAPI specification.
     *
     * This function performs a comprehensive validation of the entire HTTP request
     * based on the following sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Path parameters (if specified in specs)
     * 4. Query parameters (if specified in specs)
     *
     * @param method The HTTP method as a std::string (e.g., "POST", "PUT").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible return values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_PATH_PARAM: Invalid path parameter.
     * - ValidationError::INVALID_QUERY_PARAM: Invalid query parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, std::string& error_msg);

    /**
     * @brief Validates the entire HTTP request including JSON body against the OpenAPI specification.
     *
     * This overloaded function performs a comprehensive validation of the entire HTTP request,
     * including the JSON body, based on the following sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Body schema
     * 4. Path parameters (if specified in specs)
     * 5. Query parameters (if specified in specs)
     *
     * @param method The HTTP method as a std::string (e.g., "POST", "PUT").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param json_body The JSON body of the HTTP request as a std::string.
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible return values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_BODY: Invalid JSON body.
     * - ValidationError::INVALID_PATH_PARAM: Invalid path parameter.
     * - ValidationError::INVALID_QUERY_PARAM: Invalid query parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body, std::string& error_msg);

    /**
     * @brief Validates the entire HTTP request, including headers, against the OpenAPI specification.
     *
     * This overloaded function performs a comprehensive validation of the entire HTTP request,
     * including HTTP headers, based on the following sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Path parameters (if specified in specs)
     * 4. Query parameters (if specified in specs)
     * 5. Header parameters
     *
     * @param method The HTTP method as a std::string (e.g., "GET", "DELETE").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param headers The HTTP headers as an std::unordered_map of std::string to std::string.
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible return values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_PATH_PARAM: Invalid path parameter.
     * - ValidationError::INVALID_QUERY_PARAM: Invalid query parameter.
     * - ValidationError::INVALID_HEADER_PARAM: Invalid header parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);

    /**
     * @brief Validates the entire HTTP request, including JSON body and headers, against the OpenAPI specification.
     *
     * This overloaded function performs a comprehensive validation of the entire HTTP request,
     * including the JSON body and HTTP headers, based on the following sequence:
     * 1. HTTP method
     * 2. Route
     * 3. Body schema
     * 4. Path parameters (if specified in specs)
     * 5. Query parameters (if specified in specs)
     * 6. Header parameters
     *
     * @param method The HTTP method as a std::string (e.g., "POST", "PUT").
     * @param http_path The HTTP path as a std::string (e.g., "/api/v1/resource").
     * @param json_body The JSON body of the HTTP request as a std::string.
     * @param headers The HTTP headers as an std::unordered_map of std::string to std::string.
     * @param error_msg Reference to a std::string where the error message will be stored in case of a validation error.
     *
     * @return ValidationError enum indicating the result of the validation.
     * Possible return values include:
     * - ValidationError::NONE: No validation error.
     * - ValidationError::INVALID_METHOD: Invalid HTTP method.
     * - ValidationError::INVALID_ROUTE: Invalid route.
     * - ValidationError::INVALID_BODY: Invalid JSON body.
     * - ValidationError::INVALID_PATH_PARAM: Invalid path parameter.
     * - ValidationError::INVALID_QUERY_PARAM: Invalid query parameter.
     * - ValidationError::INVALID_HEADER_PARAM: Invalid header parameter.
     *
     * @note The error_msg argument will be populated with a JSON string in case of a validation error.
     */
    ValidationError ValidateRequest(const std::string& method, const std::string& http_path, const std::string& json_body,
                                    const std::unordered_map<std::string, std::string>& headers, std::string& error_msg);

    ~OASValidator();
};

#endif // OAS_VALIDATOR_HPP
