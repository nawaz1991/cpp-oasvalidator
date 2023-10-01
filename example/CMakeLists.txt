cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project description
project(oasvalidator-example LANGUAGES CXX)

# Create the executable
add_executable(example example.cpp)

# Link the executable with the library
target_link_libraries(example oasvalidator)

# Copy openAPI_example.json to the build directory where the example will be built
add_custom_command(TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/openAPI_example.json
        $<TARGET_FILE_DIR:example>)
