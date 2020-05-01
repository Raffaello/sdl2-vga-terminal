#include <gtest/gtest.h>
#include <vga-terminal.h>
#include <SDL2/SDL.h>

TEST(VgaTerminalSO, simpleRunningTest)
{
	// @TODO: Very basic, need improvement
	// @body: just for basic runtime using of SO.
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER));
		

	VGA_Terminal* term = VGA_TERMINAL_init();
	VGA_TERMINAL_writeXY(term, 0, 0, "test", 10, 0);
	VGA_TERMINAL_render(term);
	ASSERT_STRCASEEQ("0.2", VGA_TERMINAL_version());
	VGA_TERMINAL_destroy(term);

	SDL_Quit();
	// @todo: bot testing
	// @body: Greetings Professor Falken.

}


int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
