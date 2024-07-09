cpp-oasvalidator: REST Request Validator
============================================

[![Language C++](https://img.shields.io/badge/language-c++-blue.svg?logo=c%2B%2B)](https://isocpp.org)
[![Github releases](https://img.shields.io/github/release/nawaz1991/cpp-oasvalidator.svg)](https://github.com/nawaz1991/cpp-oasvalidator/releases)
![Mac build](https://img.shields.io/github/actions/workflow/status/nawaz1991/cpp-oasvalidator/mac-build.yml?logo=apple&label=Tests)
![Linux build](https://img.shields.io/github/actions/workflow/status/nawaz1991/cpp-oasvalidator/linux-build.yml?logo=linux&label=Tests)
![Win build](https://img.shields.io/github/actions/workflow/status/nawaz1991/cpp-oasvalidator/windows-build.yml?logo=windows&label=Tests)
[![codecov](https://codecov.io/gh/nawaz1991/cpp-oasvalidator/branch/main/graph/badge.svg?token=96b475c2-8dc1-4693-8ce3-84a572720d43)](https://codecov.io/gh/nawaz1991/cpp-oasvalidator)
[![License](https://img.shields.io/github/license/nawaz1991/cpp-oasvalidator.svg)](./LICENSE)
[![API Docs](https://img.shields.io/badge/API%20Docs-v1.1.1-brightgreen)](API.md)
[![Benchmark](https://img.shields.io/badge/Benchmark-brightgreen)](https://nawaz1991.github.io/cpp-oasvalidator/benchmark.html)


`cpp-oasvalidator` is a C++ thread-safe library engineered for the validation of HTTP requests against OpenAPI 3.x specifications. This library can be integrated with REST servers and API gateways to ensure only compliant requests reach your backend services.

With support for OpenAPI 3.x, this library streamlines the process of validating various components of an HTTP request, from methods and routes to detailed parameters and `JSON` body.

## Table of Contents
1. [Key Features](#1-key-features-)
2. [Validation Sequence](#2-validation-sequence-)
3. [Parameter Styles, data types & Deserialization Support](#3-parameter-styles-data-types--deserialization-)
   1. [Path Parameters](#31-path-parameters)
   2. [Query Parameters](#32-query-parameters)
   3. [Header Parameters](#33-header-parameters)
4. [Error Handling](#4-error-handling-)
5. [Getting Started](#5-getting-started-)
    1. [Installation](#51-installation-)
        1. [Cloning the Repository](#511-cloning-the-repository)
        2. [Building and Installing](#512-building-and-installing)
        3. [Running the Tests](#513-running-the-tests)
        4. [Generating Code Coverage Report](#514-generating-code-coverage-report)
        5. [Performance Benchmarking](#515-performance-benchmarking)
        6. [Running the Example](#516-running-the-example)
    2. [Initialization](#52--initialization-)
6. [Conclusion](#6-conclusion-)
7. [License](#7-license-)

## 1. Key Features 🌟
- **Efficient, Sequential Validation**: Validates requests in a logical order, starting from the HTTP method down to the header parameters. This means if you validate a later stage, preceding steps are validated as well.
- **Thread-Safe**: Utilizes thread-safe data structures and methods to ensure concurrent requests are handled without any issues.
- **In-Depth Error Reports**: Returns an insightful error enumeration coupled with an extensive error message in JSON format to pinpoint inaccuracies.
- **Optimized Performance**: Utilizes lazy deserialization, only processing content when all prior checks pass.
- **Broad Parameter Support**: Deserializes parameters across a spectrum of styles and data types, ensuring a wide range of OpenAPI configurations are supported.

Benchmarking results are available [here](https://nawaz1991.github.io/cpp-oasvalidator/benchmark.html).

## 2. Validation Sequence 📜
The library validates HTTP requests in the following order:

1. **HTTP Method Validation**: Ensures that the HTTP method (GET, POST, PUT, etc.) aligns with the OpenAPI spec.
2. **Route Validation**: Checks if the provided route matches the specification.
3. **Body Validation**: Validates the JSON body structure and data against the OpenAPI spec.
4. **Path Parameter Validation**: Validates path parameters.
5. **Query Parameter Validation**: Ensures query parameters are consistent with the OpenAPI spec.
6. **Header Parameter Validation**: Confirms headers are in line with the OpenAPI specification.
7. **Request Validation**: Validates the whole HTTP request starting from method, route, body (if provided), path/query params (if specified in specs) and/or headers. To address all variations, four overloaded methods are provided.

For a comprehensive understanding, refer to [API Reference](API.md).

## 3. Parameter Styles, data types & Deserialization 🛠
`cpp-oasvalidator` can deserialize and parse parameters of all data types serialized in various styles provided by Swagger/OpenAPI. Following tables provide the details.

### 3.1 Path Parameters
| **Style** | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:---------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|  simple*  |   false*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  simple   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|   label   |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|   label   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  matrix   |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  matrix   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |

&#42; Default serialization method

### 3.2 Query Parameters
|   **Style**    | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:--------------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|     form*      |    true*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|      form      |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
| spaceDelimited |    true     |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| spaceDelimited |    false    |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| pipeDelimited  |    true     |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| pipeDelimited  |    false    |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
|   deepObject   |    false    |      N/A      |    N/A     |           N/A           |         N/A          |     ✅      |

&#42; Default serialization method

### 3.3 Header Parameters
| **Style** | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:---------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|  simple*  |   false*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  simple   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |

&#42; Default serialization method

##  4. Error Handling 🚫

cpp-oasvalidator responds with a specific `validationError` enum value, indicating the error type:

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

An accompanying detailed error message, structured in JSON, elucidates the error:

```JSON
{
   "errorCode": "INVALID_BODY",
   "details": {
      "specRef": "#/paths/%2Ftest%2Fall%2F{param1}%2Fabc%2F{param2}%2F{param3}/post/requestBody/content/application%2Fjson/schema",
      "code": "oneOf",
      "description": "Property did not match any of the sub-schemas specified by 'oneOf', refer to following errors.",
      "instance": "#/field6",
      "schema": "#/properties/field6",
      "errors": [
         {
            "code": "type",
            "description": "Property has a type 'boolean' that is not in the following list: 'integer'.",
            "instance": "#/field6",
            "schema": "#/properties/field6/oneOf/0",
            "context": "oneOf"
         },
         {
            "code": "type",
            "description": "Property has a type 'boolean' that is not in the following list: 'string'.",
            "instance": "#/field6",
            "schema": "#/properties/field6/oneOf/1",
            "context": "oneOf"
         }
      ]
   }
}
```


## 5. Getting Started 🚀

### 5.1 Installation 🔧

**Prerequisites:**
- A C++11 compatible compiler.
- CMake 3.10 or higher.
- GoogleTest (for tests and code coverage)
- GCOV (for code covarge report)
- LCOV (for code covarge report)

Although `cpp-oasvalidator` utilizes RapidJSON during its build process, the final build is standalone and doesn't require any additional dependencies at runtime.

#### 5.1.1 Cloning the Repository
To clone the repository, run the following command:

```bash
git clone --recursive https://github.com/nawaz1991/cpp-oasvalidator.git
```


#### 5.1.2 Building and Installing

To build and install `cpp-oasvalidator`, follow the steps below:

1. Navigate to the root directory of the project.
2. Run the following commands:
   ```bash
    cmake -S . -B build 
    cmake --build build --target oasvalidator -j $(nproc)
    cd build
    sudo make install
   ```

#### 5.1.3 Running the Tests

To run the tests, follow the steps below:
1. Navigate to the root directory of the project.
2. Run the following commands:
   ```bash
    cmake -S . -B build -DBUILD_TESTS=ON
    cmake --build build --target oasvalidator-unittests -j $(nproc)
    build/test/unittest/oasvalidator-unittests
    ```

#### 5.1.4 Generating Code Coverage Report

To generate the code coverage report, follow the steps below:
1. Navigate to the root directory of the project.
2. Run the following commands:
   ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_COVERAGE=ON
    cmake --build build --target covhtml-oasvalidator -j $(nproc)
    ```
    The coverage report will be generated in the `build/covhtml-cpp-oasvalidator/` directory. Open `index.html` in your browser to view the report.

### 5.1.5 Performance Benchmarking

To run the performance benchmark, follow the steps below:

1. Navigate to the root directory of `cpp-oasvalidator`.
2. Run the following commands:
   ```bash
    cmake -S . -B build -DBUILD_PERF=ON
    cmake --build build --target oasvalidator-perftests -j $(nproc)
    build/test/perftest/oasvalidator-perftests
    ```

#### 5.1.6 Running the Example

To run the example, follow the steps below:

1. Navigate to the root directory of `cpp-oasvalidator`.
2. Run the following commands:
   ```bash
    cmake -S . -B build -DBUILD_EXAMPLE=ON
    cmake --build build --target oasvalidator-example -j $(nproc)
    build/example/oasvalidator-example
    ```

### 5.1.7 Generating API Documentation

To generate the API documentation, follow the steps below:

1. Navigate to the root directory of `cpp-oasvalidator`.
2. Run the following commands:
   ```bash
    cmake -S . -B build -DBUILD_DOCS=ON
    cmake --build build --target docs -j $(nproc)
    ```
    The documentation will be generated in the `build/doc/html` directory. Open `index.html` in your browser to view the documentation.

### 5.2  Initialization 🎬
To utilize `cpp-oasvalidator`, link the library and include the relevant header and initialize the validator with your OpenAPI specification:

```cmake
target_link_libraries(<your-target> oasvalidator)
```

```cpp
#include <oas_validator.hpp>
OASValidator validator("/path/to/your/spec.json");
```

> Note: The `oas_specs` can be a file path or a JSON string. If you provide a file path, the library will read the file and parse it. If you provide a JSON string, the library will parse it directly.

Then, utilize the various validation methods to inspect your requests.

For a detailed breakdown of each API, refer to the [API Reference](API.md) or the [example](example/example.cpp) provided in the repository.

##  6. Conclusion 📜
cpp-oasvalidator is your one-stop solution for rigorous REST request validation against the OpenAPI specification. With its systematic validation order, expansive parameter style support, and meticulous error reporting, it ensures your services stay compliant with your OpenAPI specs.

## 7. License 📄

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for the full license text.

© 2024 Muhammad Nawaz. All Rights Reserved.
