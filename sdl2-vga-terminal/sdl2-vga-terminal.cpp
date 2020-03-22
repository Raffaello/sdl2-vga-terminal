// sdl2-vga-terminal.cpp : Defines the entry point for the application.
//

#include "sdl2-vga-terminal.h"

using namespace std;

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

	
	//VgaTerminal term = VgaTerminal("VgaTerminal", 0, -1, 0);
	VgaTerminal term = VgaTerminal("VgaTerminal",720*2,400*2, 0, -1, 0);
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2") == SDL_FALSE) {
		cerr << "SetHint failed" << endl;
	}

	SDL_Event event;
	bool quit = false;

	//term.Write("Hello World!", 10, 0);
	for (int i = 0; i < 256; i++) {
		term.write((char)i, i, 255 - i);
	}
	term.render();
	SDL_Delay(500);
	term.writeXY(40, 12, "Again!", 9, 0);
	term.render();
	SDL_Delay(500);
	term.gotoXY(40, 24);
	while (!quit) {
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			quit = true;
		break;
		case SDL_KEYUP:
			/* Print the hardware scancode first */
			printf("Scancode: 0x%02X", event.key.keysym.scancode);
			/* Print the name of the key */
			string keyname = SDL_GetKeyName(event.key.keysym.sym);
			printf(", Name: %s", keyname.c_str());
			cout << endl;
			if (keyname == "Escape") {
				quit = true;
			}
			else if (keyname == "C") {
				cout << "clearing..." << endl;
				term.clearGrid();
				term.render();
			}
			else {
				term.write(keyname, 1, 10);
				term.render();
			}

		break;
		}
	}

	SDL_Quit();
	return 0;
}
