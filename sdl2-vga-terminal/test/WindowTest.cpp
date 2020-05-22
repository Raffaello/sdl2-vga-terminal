#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Window.hpp>
#include "Environment.hpp"
#include <exceptions/exceptions.hpp>

TEST(Window, cannotInit)
{
    Environment::tearDown();
    ASSERT_THROW(Window term = Window("", 320, 200, 0, -1, 0), exceptions::SdlWasNotInit);
    ASSERT_THROW(Window term = Window("", 0, 0, 320, 200, 0, -1, 0), exceptions::SdlWasNotInit);
    Environment::setUp();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment());
    return RUN_ALL_TESTS();
}
