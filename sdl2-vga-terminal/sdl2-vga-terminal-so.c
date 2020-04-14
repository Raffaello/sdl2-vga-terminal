/********************************************
 * USING THE SO/DLL in a C program example  *
 * The DLL/SO is linked at compile time.    *
 ********************************************/

#include <vga-terminal.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <delayimp.h>
#pragma comment(lib, "delayimp")
#endif


int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	VGA_Terminal* term = VGA_TERMINAL_init();

	VGA_TERMINAL_writeXY(term, 0, 0, "TEST test", 10, 0);
	// emulating the main even loop 3 times
	// just to show the cursor blinking....
	int quit = 3;
	while (quit--) {
		VGA_TERMINAL_render(term);
		SDL_Delay(1000);
	}

	VGA_TERMINAL_destroy(term);

#ifdef WIN32
	int t = __FUnloadDelayLoadedDLL2("vga-terminal-libd.dll");
	printf("unloaded? : %d\n", t);
	getchar();
#endif // WIN32

	SDL_Quit();
	return 0;
}
