#include <gtest/gtest.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>

#ifndef TEST_DUMP_SNAPSHOT
TEST(VgaTerminal, CannotInit) {
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
}

TEST(VgaTerminal, HelloWorld_Window) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	
	std::string title = SDL_GetWindowTitle(term.getWindow());
	ASSERT_EQ(title, termTitle);
	
	SDL_Quit();
}

TEST(VgaTerminal, HelloWorld_Text) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.write(termTitle, 7, 0);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), term.getXY());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), VgaTerminal::position_t(term.getX(), term.getY()));
	ASSERT_EQ('T', term.at(6, 0).c);

	SDL_Quit();
}

TEST(VgaTerminal, ScrollDown) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.writeXY(VgaTerminal::mode3.tw - 1, VgaTerminal::mode3.th - 1, termTitle, 7, 1);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());
	
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength - 1, VgaTerminal::mode3.th - 1), term.getXY());
	ASSERT_EQ('H', term.at(VgaTerminal::mode3.tw - 1, VgaTerminal::mode3.th - 2).c);
	ASSERT_EQ('T', term.at(5, VgaTerminal::mode3.th - 1).c);
	
	SDL_Quit();
}
#endif

int main(int argc, char** argv) {
#ifdef TEST_DUMP_SNAPSHOT
	std::cout 
		<< "******************************************************************************************************" << std::endl
		<< "*** WARNING !!! : DUMPING SCREENSHOT, IF IT IS NOT INTENDED REMOVE THE DEFINE 'TEST_DUMP_SNAPSHOT' ***" << std::endl 
		<< "***               AND RECOMPILE AGAIN.                                                             ***" << std::endl
		<< "*** WARNING !!! : THE FILE ARE AVAILABLE IN THE SUB-DIRECTORY 'snapshot' AND WILL BE REUSED FOR    ***" << std::endl
		<< "***               VERIFYING WHEN NOT DUMPING                                                       ***" << std::endl
		<< "******************************************************************************************************"
		<< std::endl;
#endif
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
