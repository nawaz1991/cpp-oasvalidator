# cpp-oasvalidator v1.0.0: API Reference
`cpp-oasvalidator` offers a comprehensive API for validating HTTP requests in accordance with OpenAPI specifications. It encompasses various validation categories including *Path, Body, Parameters, and Response*.

The API functions return a `ValidationError` type and accept a reference to a `std::string` for populating error messages in case of validation failure. A successful validation returns `ValidationError::NONE`. Otherwise, an error code is returned and the error message is populated. The error message follows this JSON schema:

```json
{
  "errorCode": "INVALID_BODY",
  "details": {
    "specRef": "#/paths/%2Fpet/put/requestBody/content/application%2Fjson/schema",
    "code": "type",
    "description": "The property type 'string' is not on the approved list: 'array'.",
    "instance": "#/photoUrls",
    "schema": "#/properties/photoUrls"
  }
}
```

`cpp-oasvalidator` defines the following `ValidationError` enum to categorize various types of validation errors:

```cpp
enum class validationError
{
    NONE                 = 0,
    INVALID_METHOD       = -1,
    INVALID_ROUTE        = -2,
    INVALID_PATH_PARAM   = -3,
    INVALID_QUERY_PARAM  = -4,
    INVALID_HEADER_PARAM = -5,
    INVALID_BODY         = -6,
    INVALID_RSP          = -7
};
```

- `errorCode`: Corresponds to the `ValidationError` enum value.
- `details`: Provides a `specRef` pointing to the exact location in the OpenAPI spec where the request failed. This is URI-encoded to handle special characters. 
  - Additional fields in `details` offer context-specific insights such as the type of error and relevant references.

Designed with performance and optimization as priorities, `cpp-oasvalidator` performs **lazy** deserialization and parsing of request components (path, query, header parameters, and body) only when all preceding validations pass.

Four overloaded methods are available to validate different combinations of request components (e.g., with/without body or headers) to accommodate various HTTP methods and use-cases.

The following API reference outlines each function and its sequence of validation checks.

## Table of Contents
1. [Constructor](#1-constructor)
2. [Validate Route](#2-validate-route-)
3. [Validate Body](#3-validate-body-)
4. [Validate Path Parameters](#4-validate-path-parameters-)
5. [Validate Query Parameters](#5-validate-query-parameters-)
6. [Validate Header Parameters](#6-validate-header-parameters-)
7. [Validate Request](#7-validate-request-)
8. [Validate Request (Overloaded)](#8-validate-request-overloaded-)
9. [Validate Request (Overloaded)](#9-validate-request-overloaded-)
10. [Validate Request (Overloaded)](#10-validate-request-overloaded-)

### 1. Constructor 🏗️
Initializes an `OASValidator` object with the OpenAPI specification from the provided file path.

##### Synopsis
```cpp
explicit OASValidator(const std::string& oas_specs);
```

##### Arguments
- `oas_specs`: The file path to the OpenAPI specification or a `JSON` string containing the OpenAPI specification.

##### Example
```cpp
OASValidator oas_validator("/path/to/openapi/spec.json");
```

##### Throws
This constructor will throw an `ValidatorInitExc` if it fails to initialize with the provided OpenAPI specification.

##### Note
Ensure that the OpenAPI specification file exists at the provided path and is in a valid `JSON` format.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 2. Validate Route 🗺️
Validates the HTTP method and path/route against the OpenAPI specification, and populates `error_msg` with details if validation fails.

##### Validation sequence
1. HTTP method
2. Route

##### Synopsis
```cpp
ValidationError ValidateRoute(
    const std::string& method,
    const std::string& http_path,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "GET", "POST") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, whereas other values indicate specific types of validation failures.
- Possible values:
  - `ValidationError::NONE`
  - `ValidationError::INVALID_METHOD`
  - `ValidationError::INVALID_ROUTE`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidateRoute("GET", "/api/v1/resource", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 3. Validate Body 📦
Validates the `JSON` body of the HTTP request against `JSON` schema the OpenAPI specification for the specific `route`, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Body schema

##### Synopsis
```cpp
ValidationError ValidateBody(
    const std::string& method,
    const std::string& http_path,
    const std::string& json_body,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `json_body`: The JSON body of the HTTP request as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_BODY`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidateBody("POST", "/api/v1/resource", "{\"key\": \"value\"}", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 4. Validate Path Parameters 📂
Validates the path parameters of the HTTP request against the OpenAPI specification, and populates `error_msg` with a descriptive message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Path parameters (in the sequence of provided in OpenAPI spec)

##### Synopsis
```cpp
ValidationError ValidatePathParam(
    const std::string& method,
    const std::string& http_path,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a `std::string`.
- `http_path`: The HTTP path with parameters (e.g., "/api/v1/resource/{id}") as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, whereas other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_PATH_PARAM`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidatePathParam("GET", "/api/v1/resource/123", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 5. Validate Query Parameters ❓
Validates the query parameters of the HTTP request against the OpenAPI specification, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Query parameters (in the sequence of provided in OpenAPI spec)

##### Synopsis
```cpp
ValidationError ValidateQueryParam(
    const std::string& method,
    const std::string& http_path,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a `std::string`.
- `http_path`: The HTTP path including query parameters (e.g., "/api/v1/resource?name=value") as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_QUERY_PARAM`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidateQueryParam("GET", "/api/v1/resource?name=value", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 6. Validate Header Parameters 📋
Validates the HTTP headers against the OpenAPI specification, and populates `error_msg` with a descriptive message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Header parameters

##### Synopsis

```cpp
ValidationError ValidateHeaders(
    const std::string& method,
    const std::string& http_path,
    const std::unordered_map<std::string, std::string>& headers,
    std::string& error_msg
);
```

##### Arguments

- `method`: The HTTP method (e.g., "GET", "POST") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `headers`: The HTTP headers as an `std::unordered_map` of `std::string` to `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns

- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_HEADER_PARAM`

##### Example
```cpp
std::unordered_map<std::string, std::string> headers = {{"Authorization", "Bearer token"}};
std::string error_msg;
ValidationError result = oas_validator.ValidateHeaders("GET", "/api/v1/resource", headers, error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 7. Validate Request ✔️
Validates the entire HTTP request including method, route, path params, and/or query params against the OpenAPI specification, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Path parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
4. Query parameters (if specified in specs, in the sequence of provided in OpenAPI spec)

##### Synopsis
```cpp
ValidationError ValidateRequest(
    const std::string& method,
    const std::string& http_path,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
  - `ValidationError::NONE`
  - `ValidationError::INVALID_METHOD`
  - `ValidationError::INVALID_ROUTE`
  - `ValidationError::INVALID_PATH_PARAM`
  - `ValidationError::INVALID_QUERY_PARAM`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidateRequest("POST", "/api/v1/resource", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 8. Validate Request (Overloaded) ✔️🔠
Validates the entire HTTP request including method, route, JSON body, path/query params (if specified) against the OpenAPI specification, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Body schema
4. Path parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
5. Query parameters (if specified in specs, in the sequence of provided in OpenAPI spec)

##### Synopsis
```cpp
ValidationError ValidateRequest(
    const std::string& method,
    const std::string& http_path,
    const std::string& json_body,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `json_body`: The JSON body of the HTTP request as a `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_BODY`
    - `ValidationError::INVALID_PATH_PARAM`
    - `ValidationError::INVALID_QUERY_PARAM`

##### Example
```cpp
std::string error_msg;
ValidationError result = oas_validator.ValidateRequest("POST", "/api/v1/resource", "{\"key\": \"value\"}", error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 9. Validate Request (Overloaded) ✔️🔢
Validates the entire HTTP request including method, route, path/query params (if specified) and headers against the OpenAPI specification, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Path parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
4. Query parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
5. Header parameters

##### Synopsis
```cpp
ValidationError ValidateRequest(
    const std::string& method,
    const std::string& http_path,
    const std::unordered_map<std::string, std::string>& headers,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `headers`: The HTTP headers as an `std::unordered_map` of `std::string` to `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_PATH_PARAM`
    - `ValidationError::INVALID_QUERY_PARAM`
    - `ValidationError::INVALID_HEADER_PARAM`

##### Example
```cpp
std::unordered_map<std::string, std::string> headers = {{"Authorization", "Bearer token"}};
std::string error_msg;
ValidationError result = oas_validator.ValidateRequest("GET", "/api/v1/resource", headers, error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.
<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 
### 10. Validate Request (Overloaded) ✔️🔣
Validates the entire HTTP request including method, route, JSON body, path/query params (if specified) and headers against the OpenAPI specification, and sets `error_msg` with a descriptive error message if validation fails.

##### Validation sequence
1. HTTP method
2. Route
3. Body schema
4. Path parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
5. Query parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
6. Header parameters

##### Synopsis

```cpp
ValidationError ValidateRequest(
    const std::string& method,
    const std::string& http_path,
    const std::string& json_body,
    const std::unordered_map<std::string, std::string>& headers,
    std::string& error_msg
);
```

##### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a `std::string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a `std::string`.
- `json_body`: The JSON body of the HTTP request as a `std::string`.
- `headers`: The HTTP headers as an `std::unordered_map` of `std::string` to `std::string`.
- `error_msg`: Reference to a `std::string` where the error message will be stored in case of a validation error.

##### Returns
- `ValidationError`: An enum indicating the result of the validation. A return value of `ValidationError::NONE` signifies successful validation, while other values indicate specific types of validation failures.
- Possible values:
    - `ValidationError::NONE`
    - `ValidationError::INVALID_METHOD`
    - `ValidationError::INVALID_ROUTE`
    - `ValidationError::INVALID_BODY`
    - `ValidationError::INVALID_PATH_PARAM`
    - `ValidationError::INVALID_QUERY_PARAM`
    - `ValidationError::INVALID_HEADER_PARAM`

##### Example
```cpp
std::unordered_map<std::string, std::string> headers = {{"Authorization", "Bearer token"}};
std::string error_msg;
ValidationError result = oas_validator.ValidateRequest("POST", "/api/v1/resource", "{\"key\": \"value\"}", headers, error_msg);

if (result != ValidationError::NONE) {
    std::cerr << "Validation failed: " << error_msg << std::endl;
}
```

##### Notes
- The `error_msg` argument will be populated with a `JSON` string in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>
