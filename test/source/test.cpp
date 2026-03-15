#include <gtest/gtest.h>
#include "command.h"

// A basic test case using Google Test
TEST(CmdProcTest, BasicAssertions) {
    // Replace with actual tests for your library functions
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}

// Example of a dummy test checking the library function
// Assuming you have declared `int dummy();` in command.h
extern int dummy();

TEST(CmdProcTest, DummyFunctionReturnsZero) {
    EXPECT_EQ(dummy(), 0);
}
