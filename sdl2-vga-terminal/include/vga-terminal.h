/***********************************************
 *** TODO:  export all the required methods  ***
 ***        just done the bare basic         ***
 ***        as a show case                   ***
 ***********************************************/

#pragma once

#include <vga-terminal-export.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	typedef void VGA_Terminal;
	VGA_TERMINAL_EXPORT const char* VGA_TERMINAL_version();
	/// create a new terminal, SDL_INIT VIDEO & TIMER must be inited first.
	VGA_TERMINAL_EXPORT VGA_Terminal* VGA_TERMINAL_init();
	/// destroy the given terminal
	VGA_TERMINAL_EXPORT void VGA_TERMINAL_destroy(VGA_Terminal* term);
	/// write in the terminal
	VGA_TERMINAL_EXPORT void VGA_TERMINAL_writeXY(VGA_Terminal* term, const uint8_t x, const uint8_t y, const char* str, const uint8_t col, const uint8_t bgCol);
	/// render in the terminal
	VGA_TERMINAL_EXPORT void VGA_TERMINAL_render(VGA_Terminal* term);
#ifdef __cplusplus
}
#endif
