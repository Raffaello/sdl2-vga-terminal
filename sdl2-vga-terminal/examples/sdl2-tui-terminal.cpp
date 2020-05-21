// VgaTerminal TUI example

#include <TuiTerminal.hpp>

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    TuiTerminal tui = TuiTerminal("Tui Example", "a retro TUI style emulator");
    int pb1 = 0;
    int pb1Max = 50;
    int pb2 = 0;
    int pb2Max = 3;
    bool quit = false;

    // the draw method has to be the first to be called
    // because of the background, otherwise
    // everything else would be overwritten... (at the moment)
    tui.draw();
    tui.drawSingleBorderDialog({ 30, 0, 40, 4 }, 11, 8, "Joshua", "GREETINGS PROFESSOR FALKEN.");
    tui.drawDialog({ 20, 7, 40, 10 }, 15, 1, "Install", "Status");

    while (!quit)
    {
        tui.progressBar(false, false, 24, 11, 20, pb2, pb2Max);
        tui.progressBar(true, true, 24, 13, 20, pb1, pb1Max);
        tui.render();

        pb1++;
        if (pb1 > pb1Max) {
            pb1 = 0;
            pb2++;
        }

        if (pb2 > pb2Max) {
            quit = true;
        }

        SDL_Delay(10);
    }

    tui.drawSingleBorderDialog({ 30, 10, 20,3 }, 15, 1, "", "Press any key...");
    tui.render();
    //press a key...
    SDL_Event event;
    bool keypressed = false;
    while (!keypressed) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_KEYDOWN) {
            keypressed = true;
        }
    }

    SDL_Quit();
    return 0;
}