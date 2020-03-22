#include <gtest/gtest.h>
#include <VgaTerminal.hpp>

TEST(Failure, CannotInit) {
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
}

TEST(Snapshot, HelloWorld) {
	SDL_Init(SDL_INIT_VIDEO);
	VgaTerminal term = VgaTerminal("Snapshot Test", 0, -1, 0);
	auto render = term.getRenderer();
	SDL_Quit();
	ASSERT_EQ(1, 2);
}

TEST(VgaTerminal, aa) {
	ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
