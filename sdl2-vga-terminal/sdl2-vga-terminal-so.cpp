// sdl2-vga-terminal.cpp : Defines the entry point for the application.
//

#include "sdl2-vga-terminal.h"

#include <vga-terminal.h>

#ifdef WIN32
#include <windows.h>
#include <delayimp.h>
#pragma comment(lib, "delayimp")

#endif

using namespace std;

int main(int argc, char* args[])
{
	cout << add(10, 1) << endl;
	cin.ignore();
#ifdef WIN32
	bool t = __FUnloadDelayLoadedDLL2("vga-terminal-libd.dll");
	cout << "unloaded? :" << t << endl;
	cin.ignore();
#endif // WIN32

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_Quit();
	return 0;
}
