#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Window.hpp>
#include <SDL2/SDL_image.h>
#include <cmath>
#include "Environment.hpp"
#include <cstring>

TEST(Window, cannotInit)
{
	Environment::tearDown();
	ASSERT_THROW(Window term = Window("", 320, 200, 0, -1, 0), std::runtime_error);
	ASSERT_THROW(Window term = Window("", 0, 0, 320, 200, 0, -1, 0), std::runtime_error);
	Environment::setUp();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new Environment());
	return RUN_ALL_TESTS();
}
