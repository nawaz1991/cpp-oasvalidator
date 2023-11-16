project(${PROJECT_NAME}-unittests)
find_package(GTest REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${RAPIDJSON_INCLUDE_DIRS}
        ${OAS_INCLUDE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:--coverage>)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        GTest::GTest
        pthread
        oasvalidator
        $<$<CONFIG:DEBUG>:-lgcov>
)

set(SPEC_FILE_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/data/openAPI_example.json")
add_definitions(-DSPEC_PATH="${SPEC_FILE_ABSOLUTE_PATH}")

