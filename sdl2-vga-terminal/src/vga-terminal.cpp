#include <vga-terminal.h>
#include <VgaTerminal.hpp>
#include <version.h>
#include <stdexcept>
#include <iostream>


using std::cerr;
using std::endl;
using std::exception;

const char* VGA_TERMINAL_version()
{
    return VGA_TERMINAL_VERSION;
}

VGA_Terminal* VGA_TERMINAL_init()
{
    VgaTerminal* term = nullptr;
    try {
       term = new VgaTerminal("test", 0, -1, 0);
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
    }

    return reinterpret_cast<VGA_Terminal*>(term);
}

void VGA_TERMINAL_destroy(VGA_Terminal* term)
{
    if (nullptr == term) {
        return;
    }

    const VgaTerminal* _term = reinterpret_cast<VgaTerminal*>(term);
    delete _term;
}

void VGA_TERMINAL_writeXY(VGA_Terminal* term, const uint8_t x, const uint8_t y, const char* str, const uint8_t col, const uint8_t bgCol)
{
    VgaTerminal* _term = reinterpret_cast<VgaTerminal*>(term);

    _term->writeXY(x, y, str, col, bgCol);
}

void VGA_TERMINAL_render(VGA_Terminal* term)
{
    VgaTerminal* _term = reinterpret_cast<VgaTerminal*>(term);
    _term->render(false);
}
