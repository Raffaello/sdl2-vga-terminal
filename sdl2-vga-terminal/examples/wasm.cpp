// sdl2-vga-terminal.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <VgaTerminal.hpp>
#include <functional>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

void main_loop(void)
{
	VgaTerminal term1 = VgaTerminal("VgaTerminal", 720, 400, 0, -1, 0);

	/*for (int i = 0; i < 256; i++) {
		term1.write((char)i, i, 255 - i);
	}*/

	term1.showCursor = false;
	term1.write("Hello World!!!", 7, 0);
	term1.render();

	//term1.writeXY(10, 15, "ษอออออออออป", 12, 3);
//term1.writeXY(10, 16, "บ         บ", 12, 3);
//term1.writeXY(10, 17, "ศอออออออออผ", 12, 3);
//term1.gotoXY(12, 16); term1.write(3, 1, 15);
//term1.gotoXY(14, 16); term1.write(4, 15, 1);
//term1.gotoXY(16, 16); term1.write(5, 0, 15);
//term1.gotoXY(18, 16); term1.write(6, 15, 0);

#ifdef EMSCRIPTEN
	emscripten_cancel_main_loop();
#endif
}

extern "C" int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	int numVideoDrivers = SDL_GetNumVideoDrivers();
	if (numVideoDrivers < 1) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "no video drivers: %s", SDL_GetError());
		return 1;
	}
	cout << "Video Drivers:" << endl;
	for (int i = 0; i < numVideoDrivers; i++) {
		std::cout << i << ". " << SDL_GetVideoDriver(i) << endl;
	}

	int numRenderDrivers = SDL_GetNumRenderDrivers();
	if (numRenderDrivers < 1) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "no render drivers: %s", SDL_GetError());
	}
	cout << "Render Drivers:" << endl;
	for (int i = 0; i < numRenderDrivers; i++) {
		SDL_RendererInfo ri;
		SDL_GetRenderDriverInfo(i, &ri);
		std::cout << i << ". " << ri.name << " --- Num Tex fmt: " << ri.num_texture_formats << endl;
	}

	int numVideoDisplay = SDL_GetNumVideoDisplays();
	if (numVideoDisplay <= 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "no Video displays: %s", SDL_GetError());
	}
	cout << "Num Video Displays:" << numVideoDisplay << endl;
	for (int i = 0; i < numVideoDisplay; i++) {
		int numDisplayModes = SDL_GetNumDisplayModes(i);
		if (numDisplayModes < 1) {
			SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "no  display mode: %s", SDL_GetError());
		}
		cout << "Display Modes of index " << i << ':' << endl;
		for (int j = 0; j < numDisplayModes; j++) {
			SDL_DisplayMode dm;
			SDL_GetDisplayMode(i, j, &dm);
			std::cout << i << "." << j << ". " << dm.w << " x " << dm.h << " Hz: " << dm.refresh_rate << " pxl: " << dm.format << endl;
		}
	}

	// --- end SDL2 system init and info


	// TODO: main loop is the problem, emscripten_set_main_loop has to be set up before
	//       something when i am creating the VgaTerminal object .... not sure
	//VgaTerminal term1 = VgaTerminal("VgaTerminal", 720, 400, 0, -1, 0);
#ifdef EMSCRIPTEN
	emscripten_set_main_loop(main_loop, 0, 1);
#endif
	
	SDL_Quit();
	return 0;
}
