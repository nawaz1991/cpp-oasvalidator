cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(OASVALIDATOR_VERSION_MAJOR "1")
set(OASVALIDATOR_VERSION_MINOR "0")
set(OASVALIDATOR_VERSION_PATCH "0")
set(OASVALIDATOR_SUMMARY "C++ library")
set(OASVALIDATOR_REPOSITORY_URL "https://github.com/nawaz1991/cpp-oasvalidator")
set(OASVALIDATOR_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(OASVALIDATOR_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(OASVALIDATOR_VERSION_STRING "${OASVALIDATOR_VERSION_MAJOR}.${OASVALIDATOR_VERSION_MINOR}.${OASVALIDATOR_VERSION_PATCH}")

project(oasvalidator LANGUAGES CXX
                         VERSION "${OASVALIDATOR_VERSION_STRING}"
                         DESCRIPTION "A FAST C++ library to validate the HTTP requests against the OpenAPI specifications of the REST server")


option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLE "Build example" OFF)
option(BUILD_COVERAGE "Build coverage" OFF)
option(BUILD_PERF "Build benchmark tests" OFF)
option(BUILD_DOCS "Build documentation" OFF)

# Default to release build type with specific optimization flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra \
        -Wunused-variable -Wunused-parameter \
        -Wunused-function -Wunused -Wno-system-headers \
        -Wno-deprecated -Woverloaded-virtual -Wwrite-strings"
    )
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops")

# Platform-specific settings
if(UNIX)
    if(APPLE)
        set(PLAT "macosx")
        set(LINK_FLAGS "-bundle -undefined dynamic_lookup -all_load")
    else()
        set(PLAT "linux")
        set(LINK_FLAGS "-shared")
    endif()
    add_compile_options(-g -Wall -fPIC)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_ARCH_NATIVE)
    if (COMPILER_SUPPORTS_ARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
elseif(WIN32)
    set(PLAT "win32")
else()
    set(PLAT "unsupported")
endif()

# Specify RapidJSON directories or fallback to default
if (NOT RAPIDJSON_INCLUDE_DIRS)
    set(RAPIDJSON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(oasvalidator SHARED ${SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

set(OAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${RAPIDJSON_INCLUDE_DIRS}
        ${OAS_INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        SOVERSION "${OASVALIDATOR_VERSION_STRING}"
        VERSION "${OASVALIDATOR_VERSION_STRING}"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_FLAGS})

# Install shared library and header
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/oas_validator.hpp"
        DESTINATION include
)

if (BUILD_COVERAGE)
    set(PROJECT_TEST_NAME ${PROJECT_NAME}-unittests)
    include(cmake/CodeCoverage.cmake)
    enable_testing()
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
            NAME covhtml-${PROJECT_NAME}
            EXECUTABLE ${PROJECT_NAME}-unittests
            BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            EXCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/*
    )
endif (BUILD_COVERAGE)

if (BUILD_TESTS OR BUILD_COVERAGE)
    add_subdirectory(test/unittest)
endif (BUILD_TESTS OR BUILD_COVERAGE)

if(BUILD_PERF)
    add_subdirectory(test/perftest)
endif(BUILD_PERF)

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif (BUILD_EXAMPLE)

if (BUILD_DOCS)
    add_subdirectory(docs)
endif (BUILD_DOCS)