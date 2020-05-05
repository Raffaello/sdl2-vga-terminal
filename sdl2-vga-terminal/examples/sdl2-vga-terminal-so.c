/********************************************
 * USING THE SO/DLL in a C program example  *
 * The DLL/SO is linked at compile time.    *
 ********************************************/

#include <vga-terminal.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#ifdef WIN32
#	include <Windows.h>
#	include <delayimp.h>
#	pragma comment(lib, "delayimp")
#endif


int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	VGA_Terminal* term = VGA_TERMINAL_init();

	VGA_TERMINAL_writeXY(term, 0, 0, "GREETINGS PROFESSOR FALKEN.", 10, 0);
	// emulating the main even loop few times
	// just to show the cursor blinking....
	int quit = 6;
	SDL_Event e;
	while (quit--) {
		VGA_TERMINAL_render(term);
		do {
			SDL_WaitEvent(&e);
		} while (e.type != SDL_USEREVENT && e.user.type != SDL_USEREVENT);
		
	}

	VGA_TERMINAL_destroy(term);

#ifdef WIN32
#	ifndef NDEBUG
#		define PFIX "d"
#	else
#		define PFIX ""
#	endif
	int t = __FUnloadDelayLoadedDLL2("vga-terminal" PFIX ".dll");
	printf("unloaded? : %d\n", t);
	getchar();
#endif // WIN32

	SDL_Quit();
	return 0;
}
