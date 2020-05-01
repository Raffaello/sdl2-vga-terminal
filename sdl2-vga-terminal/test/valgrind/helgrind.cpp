#include <iostream>
#include <VgaTerminal.hpp>
#include <cstdlib>

using namespace std;

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	// --- end SDL2 system init and info

	VgaTerminal term1 = VgaTerminal("VgaTerminal1", 0, 0, 640, 400, 0, -1, 0);
	VgaTerminal term2 = VgaTerminal("VgaTerminal2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 400, 0, -1, 0);

	
	term1.cursor_time = 100;
	term2.cursor_time = 100;

	for (int i = 0; i < 256; i++) {
		term1.write(static_cast<uint8_t>(i), i, 255 - i);
		term2.write(static_cast<uint8_t>(i), i, 255 - i);
	}

	term1.render();
	term1.writeXY(10, 15, "ษอออออออออป", 14, 2);
	term1.writeXY(10, 16, "บ         บ", 14, 2);
	term1.writeXY(10, 17, "ศอออออออออผ", 14, 2);
	term1.gotoXY(12, 16); term1.write(3, 4, 15);
	term1.gotoXY(14, 16); term1.write(4, 15, 4);
	term1.gotoXY(16, 16); term1.write(5, 0, 15);
	term1.gotoXY(18, 16); term1.write(6, 15, 0);
	term1.render();
	term1.gotoXY(40, 24);
	
	term2.writeXY(40, 10, "Terminal II", 7, 0);

	
	term1.render();
	term2.render();
	
	SDL_Event e;
	for (int i = 0; i < 10;) {
		SDL_WaitEvent(&e);
		if (e.window.windowID == term1.getWindowId()) {
			term1.render();
			i++;
		}
		else if (e.window.windowID == term2.getWindowId()) {
			term2.render();
			i++;
		}
		else {
			// ignoring
		}
	}
	
	srand(time(0));
	VgaTerminal* term;
	for (int i = 0; i < 100; i++) {
		if (i % 10) {
			term = &term1;
		}
		else {
			term = &term2;
		}

		term->write(i % 256, 14, 1);
		
		switch (rand() % 5) {
		case 0:
			term->moveCursorLeft();
			break;
		case 1:
			term->moveCursorRight();
			break;
		case 2:
			term->moveCursorUp();
			break;
		case 3:
			term->moveCursorDown();
			break;
		case 4:
			term->newLine();
			break;
		}

		if (rand() % 2) {
			term->clear();
		}

		if (rand() % 4 == 0) {
			term->write(" cursor_mode: ", 8, 7);
			switch (rand() % 4) {
			case 0:
				term->cursor_mode = VgaTerminal::CURSOR_MODE::CURSOR_MODE_NORMAL;
				term->write("NORMAL", 15, 10);
				break;
			case 1:
				term->cursor_mode = VgaTerminal::CURSOR_MODE::CURSOR_MODE_FAT;
				term->write("FAT", 15, 10);
				break;
			case 2:
				term->cursor_mode = VgaTerminal::CURSOR_MODE::CURSOR_MODE_BLOCK;
				term->write("BLOCK", 15, 10);
				break;
			case 3:
				term->cursor_mode = VgaTerminal::CURSOR_MODE::CURSOR_MODE_VERTICAL;
				term->write("VERTICAL", 15, 10);
				break;
			}
		}

		if (rand() % 5) {
			term->resetViewport();
			term->setViewPort(term->getViewport());
			term->resetViewport();
		}

		if (rand() % 2) {
			term->cursor_time = rand() % 1000;
			term->write(" cursor time:", 15, 1);
			term->write(std::to_string(term->cursor_time).c_str(), 15, 2);
		}
		

		term->render();
	}

	SDL_Quit();
	return 0;
}
