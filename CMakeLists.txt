cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cmd-proc LANGUAGES CXX)

# === Options ===
option(CMD_PROC_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(CMD_PROC_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# === Library ===
set(SOURCES
    "source/command.cpp"
)

add_library(cmd-proc ${SOURCES})

# Setup include directories for both build and install interfaces
target_include_directories(cmd-proc PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
    $<INSTALL_INTERFACE:include>
)

# === Compiler Warnings ===
set(COMPILER_WARNINGS
    $<IF:$<CXX_COMPILER_ID:MSVC>,
        /W4 /WX,
        -Wall -Wextra -Wpedantic $<IF:$<BOOL:${CMD_PROC_WARNINGS_AS_ERRORS}>,-Werror,>
    >
)
target_compile_options(cmd-proc PRIVATE ${COMPILER_WARNINGS})

# === Coverage ===
if(CMD_PROC_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(cmd-proc PRIVATE --coverage -O0 -g)
        target_link_options(cmd-proc PUBLIC --coverage)
    endif()
endif()

# === Testing & GTest from Source ===
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows: prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
endif()
