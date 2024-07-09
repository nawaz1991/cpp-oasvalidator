cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Version and other settings
set(OASVALIDATOR_VERSION_MAJOR "1")
set(OASVALIDATOR_VERSION_MINOR "1")
set(OASVALIDATOR_VERSION_PATCH "1")
set(OASVALIDATOR_VERSION_STRING "${OASVALIDATOR_VERSION_MAJOR}.${OASVALIDATOR_VERSION_MINOR}.${OASVALIDATOR_VERSION_PATCH}")
set(OASVALIDATOR_SUMMARY "C++ library")
set(OASVALIDATOR_REPOSITORY_URL "https://github.com/nawaz1991/cpp-oasvalidator")
set(OASVALIDATOR_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(OASVALIDATOR_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
message(STATUS "Building cpp-oasvalidator version ${OASVALIDATOR_VERSION_STRING}")

project(cpp-oasvalidator LANGUAGES CXX VERSION ${OASVALIDATOR_VERSION_STRING} DESCRIPTION "A FAST C++ library to validate the HTTP requests against the OpenAPI specifications of the REST server")

include(GNUInstallDirs)

# Include the SetCompilerFlags module
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SetCompilerFlags)

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

option(OASVALIDATOR_BUILD_CXX11 "Build cpp-oasvalidator with C++11" OFF)
option(OASVALIDATOR_BUILD_CXX17 "Build cpp-oasvalidator with C++17" ON)

if (OASVALIDATOR_BUILD_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    message(STATUS "Building with C++11")
elseif (OASVALIDATOR_BUILD_CXX17)
    set(CMAKE_CXX_STANDARD 17)
    message(STATUS "Building with C++17")
endif ()
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Specify RapidJSON directories or fallback to default
if (NOT RAPIDJSON_INCLUDE_DIRS)
    set(RAPIDJSON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include")
endif ()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add library
set(OASVALIDATOR oasvalidator)
if (BUILD_SHARED_LIB)
    add_library(${OASVALIDATOR} SHARED ${SOURCES})
else ()
    add_library(${OASVALIDATOR} STATIC ${SOURCES})
endif ()

# Include directories
set(OAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(${OASVALIDATOR}
        PRIVATE
        ${RAPIDJSON_INCLUDE_DIRS}
        ${OAS_INCLUDE_DIR}
)

set_target_properties(${OASVALIDATOR} PROPERTIES
        SOVERSION "${OASVALIDATOR_VERSION_STRING}"
        VERSION "${OASVALIDATOR_VERSION_STRING}"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Apply compiler flags
set_compiler_flags(${OASVALIDATOR})

# Add extra search paths for libraries and includes
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "The directory the headers are installed in")
set(DOC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${OASVALIDATOR}" CACHE PATH "Path to the documentation")

set(CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/OASValidator" CACHE PATH "The directory cmake files are installed in")

# Installation
install(TARGETS ${OASVALIDATOR}
        EXPORT ${OASVALIDATOR}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES "${OAS_INCLUDE_DIR}/oas_validator.hpp"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES README.md LICENSE
        DESTINATION "${DOC_INSTALL_DIR}"
        COMPONENT doc
)

# Export package for use from the build tree
export(EXPORT ${OASVALIDATOR}-targets
        FILE "${CMAKE_BINARY_DIR}/${OASVALIDATOR}Targets.cmake"
        NAMESPACE OASValidator::
)

# Create and install the Config.cmake and ConfigVersion.cmake files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/OASValidatorConfigVersion.cmake"
        VERSION ${OASVALIDATOR_VERSION_STRING}
        COMPATIBILITY AnyNewerVersion
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/OASValidatorConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/OASValidatorConfig.cmake"
        @ONLY)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/OASValidatorConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/OASValidatorConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_DIR}"
)

install(EXPORT ${OASVALIDATOR}-targets
        FILE OASValidatorTargets.cmake
        NAMESPACE OASValidator::
        DESTINATION ${CMAKE_INSTALL_DIR}
)

# Build coverage
if (BUILD_COVERAGE)
    set(TEST_TARGET ${OASVALIDATOR}-unittests)
    set(DOWNLOADED_CMAKE_MODULES "${CMAKE_BINARY_DIR}/downloaded_cmake_modules")
    file(MAKE_DIRECTORY ${DOWNLOADED_CMAKE_MODULES})
    set(CODE_COVERAGE_SCRIPT "https://raw.githubusercontent.com/bilke/cmake-modules/70a0b5209a2a7cbea181b148ca645e4181f394bc/CodeCoverage.cmake")
    file(DOWNLOAD ${CODE_COVERAGE_SCRIPT} "${DOWNLOADED_CMAKE_MODULES}/CodeCoverage.cmake")
    include("${DOWNLOADED_CMAKE_MODULES}/CodeCoverage.cmake")
    enable_testing()
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
            NAME covhtml-${OASVALIDATOR}
            EXECUTABLE ${TEST_TARGET}
            BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            EXCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/*
            EXCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/build/*
    )
endif ()

# Build tests
if (BUILD_TESTS OR BUILD_COVERAGE)
    add_subdirectory(test/unittest)
    include(CTest)
endif ()

# Build performance tests
if (BUILD_PERF)
    # Use thirdparty/benchmark as a subproject
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable benchmark install")
    set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "Disable benchmark docs install")
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests")
    set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "Disable benchmark bundled GTest")
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable GTest in benchmark")
    add_subdirectory(thirdparty/benchmark)

    add_subdirectory(test/perftest)
endif ()

# Build example
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

# Build documentation
if (BUILD_DOCS)
    add_subdirectory(docs)
endif ()
