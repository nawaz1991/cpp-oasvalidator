project(${PROJECT_NAME}-unittests LANGUAGES CXX)

set(GOOGLETEST_VERSION 1.14.0)

find_package(GTestSrc)

IF(GTESTSRC_FOUND)
    enable_testing()

    if (WIN32 AND (NOT CYGWIN) AND (NOT MINGW))
        set(gtest_disable_pthreads ON)
        set(gtest_force_shared_crt ON)
    endif()

    add_subdirectory(${GTEST_SOURCE_DIR} ${CMAKE_BINARY_DIR}/googletest)
    include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

    set(TEST_LIBRARIES gtest gtest_main)


    file(GLOB_RECURSE SOURCES "src/*.cpp")

    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
        endif()
    endif(CCACHE_FOUND)


    add_executable(${PROJECT_NAME} ${SOURCES})

    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${RAPIDJSON_INCLUDE_DIRS}
            ${OAS_INCLUDE_DIR}
    )

    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:--coverage>)

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            oasvalidator
            ${TEST_LIBRARIES}
            $<$<CONFIG:DEBUG>:-lgcov>
    )

    add_test(NAME unittest
            COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/oasvalidator-unittest
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

    set(SPEC_FILE_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/data/openAPI_example.json")
    add_definitions(-DSPEC_PATH="${SPEC_FILE_ABSOLUTE_PATH}")

ENDIF(GTESTSRC_FOUND)