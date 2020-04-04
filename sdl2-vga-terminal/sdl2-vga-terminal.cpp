// sdl2-vga-terminal.cpp : Defines the entry point for the application.
//

#include "sdl2-vga-terminal.h"

using namespace std;

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
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
	VgaTerminal term2 = VgaTerminal("T2 test", 640, 400, 0, -1, 0);
	term2.write("events: understand which windows is focused...", 10, 0);
	term2.render();
	SDL_Delay(1000);

	VgaTerminal term1 = VgaTerminal("VgaTerminal",720,400, 0, -1, 0);
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2") == SDL_FALSE) {
		cerr << "SetHint failed" << endl;
	}

	SDL_Event event;
	bool quit = false;

	for (int i = 0; i < 256; i++) {
		term1.write((char)i, i, 255 - i);
	}

	term1.render();
	SDL_Delay(500);
	term1.writeXY(40, 12, "Again!", 9, 0);
	term1.render();
	SDL_Delay(500);
	term1.writeXY(10, 15, "ษอออออออออป",12 ,3);
	term1.writeXY(10, 16, "บ         บ",12 ,3);
	term1.writeXY(10, 17, "ศอออออออออผ",12 ,3);
	term1.gotoXY(12, 16); term1.write(3, 1, 15);
	term1.gotoXY(14, 16); term1.write(4, 15, 1);
	term1.gotoXY(16, 16); term1.write(5, 0, 15);
	term1.gotoXY(18, 16); term1.write(6, 15, 0);
	term1.render();
	term1.gotoXY(40, 24);
	term1.render();
	string keyname;
	while (!quit) {
		
		SDL_WaitEvent(&event);
		VgaTerminal* term;
		if (event.window.windowID == term1.getWindowId()) {
			term = &term1;
		}
		else if (event.window.windowID == term2.getWindowId()) {
			term = &term2;
		}
		else {
			term1.render();
			term2.render();
			continue;
		}

		switch (event.type) {
		case SDL_USEREVENT:
			SDL_UserEvent userevent = event.user;
			if (userevent.code == 0) {
				cout << "cursor!" << endl;
			}
			break;
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			keyname = SDL_GetKeyName(event.key.keysym.sym);
			if (keyname == "F11") {
				term->toggleFullscreenDesktop();
				term->render(true);
			}
			else if (keyname == "Left") {
				term->moveCursorLeft();
			}
			else if (keyname == "Right") {
				term->moveCursorRight();
			}
			else if (keyname == "Up") {
				term->moveCursorUp();
			}
			else if (keyname == "Down") {
				term->moveCursorDown()
					;
			}
			break;
		case SDL_KEYUP:
			/* Print the hardware scancode first */
			printf("Scancode: 0x%02X", event.key.keysym.scancode);
			/* Print the name of the key */
			keyname = SDL_GetKeyName(event.key.keysym.sym);
			printf(", Name: %s", keyname.c_str());
			cout << endl;
			if (keyname == "Escape") {
				quit = true;
			}
			else if (keyname == "C") {
				cout << "clearing..." << endl;
				term->clearGrid();
			}
			else {
				term->write(keyname, 1, 10);
			}

			break;
		}
		
		term1.render();
		term2.render();
	}

	SDL_Quit();
	return 0;
}
