#include <vga-terminal.h>
#include <VgaTerminal.hpp>

VGA_Terminal* VGA_TERMINAL_init()
{
	VgaTerminal* _term = new VgaTerminal("test", 0, -1, 0);
	return reinterpret_cast<VGA_Terminal*>(_term);
}

void VGA_TERMINAL_destroy(VGA_Terminal* term)
{
	if (nullptr == term) {
		return;
	}

	VgaTerminal* _term = reinterpret_cast<VgaTerminal*>(term);
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