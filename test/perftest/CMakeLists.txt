project(${OASVALIDATOR}-perftests LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        benchmark
        oasvalidator
)

set(SPEC_FILE_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/data/openAPI_example.json")
add_definitions(-DSPEC_PATH="${SPEC_FILE_ABSOLUTE_PATH}")
