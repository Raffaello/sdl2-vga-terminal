#include <gtest/gtest.h>
#include <vga-terminal.h>
#include <SDL2/SDL.h>

TEST(DISABLED_VgaTerminalSO, simpleRunningTest)
{
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER));
		

	VGA_Terminal* term = VGA_TERMINAL_init();
	VGA_TERMINAL_writeXY(term, 0, 0, "test", 10, 0);
	VGA_TERMINAL_render(term);
	VGA_TERMINAL_destroy(term);

	SDL_Quit();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}