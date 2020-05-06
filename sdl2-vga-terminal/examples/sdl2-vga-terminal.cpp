// sdl2-vga-terminal.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <VgaTerminal.hpp>


using namespace std;

int main(int argc, char* argv[])
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

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG);
	// --- end SDL2 system init and info
	VgaTerminal term2 = VgaTerminal("T2 test", 640, 400, 0, -1, 0);
	term2.cursor_mode = VgaTerminal::CURSOR_MODE::CURSOR_MODE_BLOCK;
	term2.write("events: understand which windows is focused...", 10, 0);
	term2.writeXY(10, 1, "**** Viewporting ****", 10, 0);
	term2.writeXY(10, 2, "*                   *", 10, 0);
	term2.writeXY(10, 3, "*                   *", 10, 0);
	term2.writeXY(10, 4, "*                   *", 10, 0);
	term2.writeXY(10, 5, "*                   *", 10, 0);
	term2.writeXY(10, 6, "*                   *", 10, 0);
	term2.writeXY(10, 7, "*********************", 10, 0);

	term2.setViewPort(11, 2, 19, 5);
	term2.write("Hello ViewPort !!!", 1, 14);
	term2.render();
	SDL_Delay(1000);

	VgaTerminal term1 = VgaTerminal("VgaTerminal",720,400, 0, -1, 0);
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2") == SDL_FALSE) {
		cerr << "SetHint failed" << endl;
	}

	SDL_Event event;
	bool quit = false;

	for (int i = 0; i < 256; i++) {
		term1.write(static_cast<uint8_t>(i), i, 255 - i);
	}

	cout << "Keyboards specials on the focused window: " << endl
		<< "Press F11 to toggle fullscreen" << endl
		<< "press C to clear"
		<< "press S to change cursor shape" << endl
		<< "press + / Keypad + to increase cursor blinking speed" << endl
		<< "press - / Keypad - to decrease cursor blinkgin speed" << endl
		<< "[Viewport Window only] up/down/left/right arrows to move the cursor" << endl;

	term1.render();
	SDL_Delay(500);
	term1.writeXY(40, 12, "Again!", 9, 15);
	term1.render();
	SDL_Delay(500);
	term1.writeXY(10, 15, "ÉÍÍÍÍÍÍÍÍÍ»", 14 , 2);
	term1.writeXY(10, 16, "º         º", 14 , 2);
	term1.writeXY(10, 17, "ÈÍÍÍÍÍÍÍÍÍ¼", 14 , 2);
	term1.gotoXY(12, 16); term1.write(3, 4, 15);
	term1.gotoXY(14, 16); term1.write(4, 15, 4);
	term1.gotoXY(16, 16); term1.write(5, 0, 15);
	term1.gotoXY(18, 16); term1.write(6, 15, 0);
	term1.render();
	term1.gotoXY(40, 24);
	term1.render();
	
	Window::handler_t f = [](const SDL_Event& event, Window* w) {
		VgaTerminal* term = reinterpret_cast<VgaTerminal*>(w);
		SDL_UserEvent userevent;
		string keyname;
		switch (event.type) {
		case SDL_KEYDOWN:
			keyname = SDL_GetKeyName(event.key.keysym.sym);
			if (keyname == "Left") {
				term->moveCursorLeft();
			}
			else if (keyname == "Right") {
				term->moveCursorRight();
			}
			else if (keyname == "Up") {
				term->moveCursorUp();
			}
			else if (keyname == "Down") {
				term->moveCursorDown();
			}
		break;
		case SDL_USEREVENT:
			userevent = event.user;
			//  at some point a better event name should be used
			if (userevent.type == SDL_USEREVENT && userevent.code == 0) {
                              // the commented code below is intended as an example
				//std::cout << "cursor event!" << endl;
			}
			else {
				//cout << userevent.code << endl;
			}
		break;
		}

		return true;
	};

	term2.handler = f;
	term2.cursor_time = 100;
	
	string keyname;
	while (!quit) {
		SDL_WaitEvent(&event);
		term2.handleEvent(event);
		term1.handleEvent(event);

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
		
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			keyname = SDL_GetKeyName(event.key.keysym.sym);
			if (keyname == "F11") {
				term->toggleFullscreenDesktop();
				term->render(true);
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
				term->clear();
			}
			else if (keyname == "S") {
				cout << "cursor shape..." << endl;
				// unsafe cursor rotation, please be a little bit more verbose
				// and handle properly the cases.
				auto cs = (static_cast<int>(term->cursor_mode) + 1) % VgaTerminal::NUM_CURSOR_MODES;
				term->cursor_mode = static_cast<VgaTerminal::CURSOR_MODE>(cs);
			}
			else if ((event.key.keysym.sym == SDLK_EQUALS && event.key.keysym.mod & KMOD_SHIFT) || keyname == "Keypad +") {
				term->cursor_time = static_cast<uint16_t>(round(term->cursor_time * 0.9));
				cout << "incresing cursor speed (decrease cursor_time) ~10% = " << term->cursor_time << endl;
			}
			else if ((keyname == "Keypad -") || (keyname == "-")) {
				term->cursor_time = static_cast<uint16_t>(round(term->cursor_time * 1.1));
				cout << "decresing cursor speed (incresing cursor_time) ~10% = " << term->cursor_time << endl;
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
