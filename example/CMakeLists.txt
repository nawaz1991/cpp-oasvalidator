cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project description
project(oasvalidator-example LANGUAGES CXX)

# Create the executable
add_executable(example example.cpp)
find_package(Threads REQUIRED)
# Link the executable with the library
target_link_libraries(example oasvalidator Threads::Threads)

set(SPEC_FILE_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/data/openAPI_example.json")
add_definitions(-DSPEC_PATH="${SPEC_FILE_ABSOLUTE_PATH}")
