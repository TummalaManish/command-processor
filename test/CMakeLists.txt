set(SOURCES
    "source/test.cpp"
)

add_executable(test_cmd_proc ${SOURCES})

# Link against our library and GTest
target_link_libraries(test_cmd_proc PRIVATE cmd-proc GTest::gtest_main)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(test_cmd_proc)

# Apply coverage flags to the test executable as well
if(CMD_PROC_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_cmd_proc PRIVATE --coverage -O0 -g)
        target_link_options(test_cmd_proc PUBLIC --coverage)
    endif()
endif()
