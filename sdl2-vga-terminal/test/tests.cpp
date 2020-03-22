#include <gtest/gtest.h>
#include <VgaTerminal.hpp>

TEST(VgaTerminal, CannotInit) {
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
}

TEST(VgaTerminal, HelloWorld) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, 0, -1, 0);
	
	std::string title = SDL_GetWindowTitle(term.getWindow());
	ASSERT_EQ(title, termTitle);

	//term.getRenderer();
	//term.render
	SDL_Quit();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
