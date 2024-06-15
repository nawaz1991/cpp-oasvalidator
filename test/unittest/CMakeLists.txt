project(${OASVALIDATOR}-unittests LANGUAGES CXX)

set(GOOGLETEST_VERSION 1.14.0)

# Include the custom FindGTestSrc.cmake script
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
find_package(GTestSrc REQUIRED)

IF (GTESTSRC_FOUND)
    enable_testing()

    if (WIN32 AND (NOT CYGWIN) AND (NOT MINGW))
        set(gtest_disable_pthreads ON)
        set(gtest_force_shared_crt ON)
    endif ()

    add_subdirectory(${GTEST_SOURCE_DIR} ${CMAKE_BINARY_DIR}/googletest)
    include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

    set(TEST_LIBRARIES gtest gtest_main)

    file(GLOB_RECURSE SOURCES "src/*.cpp")

    find_program(CCACHE_FOUND ccache)
    if (CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
        endif ()
    endif (CCACHE_FOUND)

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

    add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

    set(SPEC_FILE_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/data/openAPI_example.json")
    add_definitions(-DSPEC_PATH="${SPEC_FILE_ABSOLUTE_PATH}")

ENDIF (GTESTSRC_FOUND)
