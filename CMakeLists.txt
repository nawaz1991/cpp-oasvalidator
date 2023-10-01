cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(cpp-oasvalidator LANGUAGES CXX)

# Default to release build type with specific optimization flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -funroll-loops")

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
    set(RAPIDJSON_INCLUDE_DIRS rapidjson/include)
endif()

# Use target-based commands
file(GLOB SOURCES "src/*.cpp")

# Create a shared library
add_library(oasvalidator SHARED ${SOURCES})

target_compile_features(oasvalidator PRIVATE cxx_std_11)

target_include_directories(oasvalidator
        PRIVATE
        ${RAPIDJSON_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Set output directory for the library
set_target_properties(oasvalidator PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Link platform-specific flags
target_link_libraries(oasvalidator PRIVATE ${LINK_FLAGS})

# Install shared library and header
install(TARGETS oasvalidator
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/oas_validator.hpp"
        DESTINATION include
)