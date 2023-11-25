cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
if (POLICY CMP0025)
    # detect Apple's Clang
    cmake_policy(SET CMP0025 NEW)
endif ()
if (POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif ()
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) 
endif()

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(OASVALIDATOR_VERSION_MAJOR "1")
set(OASVALIDATOR_VERSION_MINOR "0")
set(OASVALIDATOR_VERSION_PATCH "0")
set(OASVALIDATOR_VERSION_STRING "${OASVALIDATOR_VERSION_MAJOR}.${OASVALIDATOR_VERSION_MINOR}.${OASVALIDATOR_VERSION_PATCH}")
set(OASVALIDATOR_SUMMARY "C++ library")
set(OASVALIDATOR_REPOSITORY_URL "https://github.com/nawaz1991/cpp-oasvalidator")
set(OASVALIDATOR_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(OASVALIDATOR_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
message(STATUS "Building oasvalidator version ${OASVALIDATOR_VERSION_STRING}")

project(oasvalidator LANGUAGES CXX
        VERSION "${OASVALIDATOR_VERSION_STRING}"
        DESCRIPTION "A FAST C++ library to validate the HTTP requests against the OpenAPI specifications of the REST server")

# Default to release build type with specific optimization flags
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLE "Build example" OFF)
option(BUILD_COVERAGE "Build coverage" OFF)
option(BUILD_PERF "Build benchmark tests" OFF)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_SHARED_LIB "Build using shared libraries" ON)

option(OASVALIDATOR_BUILD_CXX11 "Build oasvalidator with C++11" OFF)
option(OASVALIDATOR_BUILD_CXX17 "Build oasvalidator with C++17" ON)
if (OASVALIDATOR_BUILD_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
elseif (OASVALIDATOR_BUILD_CXX17)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
endif()

message(STATUS "EXTRA_CXX_FLAGS: ${EXTRA_CXX_FLAGS}")

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    message(STATUS "Using ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    endif ()
endif()

############################# Compiler-specific settings #############################
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GNU compiler")
    if (NOT CMAKE_CROSSCOMPILING)
        message(STATUS "Using native CPU optimizations")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wsign-conversion)
    if (OASVALIDATOR_BUILD_CXX11)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0")
            message(STATUS "Building with C++0x")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        else ()
            message(STATUS "Building with C++11")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
    elseif (OASVALIDATOR_BUILD_CXX17 AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
        message(STATUS "Building with C++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using Clang compiler")
    if (NOT CMAKE_CROSSCOMPILING)
        message(STATUS "Using native CPU optimizations")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-missing-field-initializers")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wimplicit-fallthrough)
    if (OASVALIDATOR_BUILD_CXX11)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.3")
            message(STATUS "Building with C++0x")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        else ()
            message(STATUS "Building with C++11")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
    elseif (OASVALIDATOR_BUILD_CXX17 AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0")
        message(STATUS "Building with C++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC compiler")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    add_definitions(-DNOMINMAX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    if (OASVALIDATOR_BUILD_CXX11 AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.10")
        message(STATUS "Building with C++11")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11")
    elseif (OASVALIDATOR_BUILD_CXX17)
        message(STATUS "Building with C++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    endif ()
    # Always compile with /WX
    if (CMAKE_CXX_FLAGS MATCHES "/WX-")
        string(REGEX REPLACE "/WX-" "/WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
    message(STATUS "Using XL compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qarch=auto")
endif ()
# if release build, add extra optimization flags
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Adding extra optimization flags for release build")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -fomit-frame-pointer -DNDEBUG -fno-strict-aliasing -fvisibility-inlines-hidden")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /Ob2 /Oi /Ot /Oy /GL /fp:fast /GS- /Gy /DNDEBUG")
    endif()
endif()

#add extra search paths for libraries and includes
SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Directory where lib will install")
SET(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}" CACHE PATH "Path to the documentation")

if (UNIX OR CYGWIN)
    SET(_CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
elseif (WIN32)
    SET(_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
endif ()
SET(CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" CACHE PATH "The directory cmake files are installed in")

# Specify RapidJSON directories or fallback to default
if (NOT RAPIDJSON_INCLUDE_DIRS)
    set(RAPIDJSON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

if (BUILD_SHARED_LIB)
    add_library(oasvalidator SHARED ${SOURCES})
else ()
    add_library(oasvalidator STATIC ${SOURCES})
endif ()

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

############################# Installation #############################
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
install(FILES "${OAS_INCLUDE_DIR}/oas_validator.hpp"
        DESTINATION include
)
install(FILES README.md
        DESTINATION "${DOC_INSTALL_DIR}"
        COMPONENT doc)
install(FILES LICENSE
        DESTINATION "${DOC_INSTALL_DIR}"
        COMPONENT doc)

############################# Subprojects #############################
# Export package for use from the build tree
EXPORT(PACKAGE ${PROJECT_NAME})
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
endif ()

if (BUILD_TESTS OR BUILD_COVERAGE)
    add_subdirectory(test/unittest)
endif ()

if(BUILD_PERF)
    # Use thidpart/benchmark as a subproject
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable benchmark install")
    set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "Disable benchmark docs install")
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests")
    set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "Disable benchmark bundled GTest")
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable GTest in benchmark")
    add_subdirectory(thirdparty/benchmark)

    add_subdirectory(test/perftest)
endif ()

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

if (BUILD_DOCS)
    add_subdirectory(docs)
endif ()