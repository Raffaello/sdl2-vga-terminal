#include "TuiTerminal.hpp"
#include <algorithm>
#include "tui/utils.hpp"


TuiTerminal::TuiTerminal(const std::string& title, const std::string& description) :
	_title(title), _desc(description), _term(VgaTerminal(title))
{
	_term.autoScroll = false;
	_term.showCursor = false;
}

SDL_Window* TuiTerminal::getWindow()
{
	return _term.getWindow();
}

SDL_Renderer* TuiTerminal::getRenderer()
{
	return _term.getRenderer();
}

void TuiTerminal::draw()
{
	_term.resetViewport();
	_drawBackGround();
	_drawHeader();
	_drawFooter();
	_term.setViewPort(0, 5, _term.getMode().tw, _term.getMode().th - 5);
}

void TuiTerminal::render()
{
	_term.render();
}

void TuiTerminal::_drawBackGround() noexcept
{
	_term.fill(background_tc.c, background_tc.col, background_tc.bgCol);
}

void TuiTerminal::_drawHeader()
{
	constexpr uint8_t x = 4;
	constexpr uint8_t y = 0;
	constexpr uint8_t w = 72;
	constexpr uint8_t h = 3;
	constexpr uint8_t col = 14;
	constexpr uint8_t bgCol = 1;
	constexpr SDL_Rect r{ x, y, w, h };
	
	const std::string version = std::string("Version ") + VERSION;
	auto [lmark, rmark] = tui::align3Strings(_title, _desc, version, w);
	const SDL_Rect rl = { x, y, lmark, h };
	const SDL_Rect rm = { x+lmark-1, y, rmark-lmark+1, h };
	const SDL_Rect rr = { x+rmark-1, y, w-rmark+1, h };

	_drawLeftBorder(rl, col, bgCol, _title);
	_drawMiddleBorder(rm, col, bgCol, _desc);
	_drawRightBorder(rr, col, bgCol, version);
	_drawShadow(r);
}

void TuiTerminal::drawSingleBorderDialog(const SDL_Rect& r, uint8_t col, uint8_t bgCol, const std::string& title, const std::string& body)
{
	if (r.h < 3) {
		throw std::invalid_argument("height must be 3 at least");
	}

	if (r.w < title.size() + 2) {
		throw std::invalid_argument("title too long");
	}

	if (r.w < body.size() + 2) {
		throw std::invalid_argument("body too long");
	}


	std::string padTitle = tui::alignString(title, r.w - 2 - title.size(), 196);
	std::string padBody = tui::alignString(body, r.w - 2 - body.size(), ' ');
	// Top
	_term.gotoXY(r.x, r.y);
	_term.write(218, col, bgCol);
	_term.write(padTitle, col, bgCol);
	_term.write(191, col, bgCol);
	// Middle
	_term.gotoXY(r.x, r.y + 1);
	_term.write(179, col, bgCol);
	_term.write(padBody, col, bgCol);
	_term.write(179, col, bgCol);
	for (int i = 2; i < r.h - 1; i++)
	{
		_term.gotoXY(r.x, r.y + i);
		_term.write(179, col, bgCol);
		for (int j = 2; j < r.w; j++) {
			_term.write(' ', col, bgCol);
		}
		_term.write(179, col, bgCol);
	}
	// Bottom
	_term.gotoXY(r.x, r.y + r.h - 1);
	_term.write(192, col, bgCol);
	_drawHBorder(r.x + 1, r.y + r.h - 1, r.w, col, bgCol, 196);
	_term.write(217, col, bgCol);
	_drawShadow(r);
}

void TuiTerminal::drawDialog(const SDL_Rect& r, uint8_t col, uint8_t bgCol, const std::string& header, const std::string& body)
{
	if (r.h < 5) {
		throw std::invalid_argument("height must be 3 at least");
	}

	if (r.w < header.size() - 2) {
		throw std::invalid_argument("title too long");
	}

	if (r.w < body.size() - 2) {
		throw std::invalid_argument("body too long");
	}

	std::string padTitle = tui::alignString(header, r.w - 2 - header.size(), 196);
	std::string padBody = tui::alignString(body, r.w - 2 - body.size(), ' ');
	
	// Header
	// Top
	_term.gotoXY(r.x, r.y);
	_term.write(218, col, bgCol);
	_term.write(padTitle, col, bgCol);
	_term.write(183, col, bgCol);
	// Middle
	_term.gotoXY(r.x, r.y + 1);
	_term.write(179, col, bgCol);
	_term.write(padBody, col, bgCol);
	_term.write(186, col, bgCol);
	// Bottom
	_term.gotoXY(r.x, r.y + 2);
	_term.write(195, col, bgCol);
	_drawHBorder(r.x + 1, r.y + 2, r.w, col, bgCol, 196);
	_term.write(182, col, bgCol);
	// End Header

	// Body
	for (int i = 3; i < r.h - 1; i++)
	{
		_term.gotoXY(r.x, r.y + i);
		_term.write(179, col, bgCol);
		for (int j = 2; j < r.w; j++) {
			_term.write(' ', col, bgCol);
		}
		_term.write(186, col, bgCol);
	}
	// Bottom
	_term.gotoXY(r.x, r.y + r.h - 1);
	_term.write(212, col, bgCol);
	_drawHBorder(r.x + 1, r.y + r.h - 1, r.w, col, bgCol, 205);
	_term.write(188, col, bgCol);
	_drawShadow(r);

}

// progress [0, max]
void TuiTerminal::progressBar(const bool colored, const bool showProgress, const uint8_t x, const uint8_t y, const uint8_t width, const size_t progress, const size_t max)
{
	// TODO progressBar algorithm is not optimal
	// BODY there are wasting operations in
	// BODY drawing the same cell each call.
	// BODY
	// BODY ---
	// BODY
	// BODY - clear the line should not required.
	// BODY - if not colored, just check if need to fill the "next cell"
	// BODY - if colored redraw only when there is a change in color from previous call.
	// BODY - rewrite always the showProgress (promote to a label and here generate an event?)
	// -----------------
	
	// red until 33%
	// yellow until 67%
	// green until 100%

	// TODO as parameter,
	// BODY ... from parent container..
	constexpr uint8_t _col = 15;
	constexpr uint8_t bgCol = 1;
	constexpr int bufSize = 5;
	uint8_t _length;
	
	// width > bufsize if colored && _length >= 3 
	if (showProgress) {
		_length = width - bufSize;
	}
	else {
		_length = width;
	}

	// this is sub-optimal, it is drawing twice, can be done exactly once
	_drawHBorder(x, y, width, _col, bgCol, ' ');
	_term.gotoXY(x, y);
	// X = X:length = progress:max => x*max/100 = progress => X = progress*100/max
	const double ratio = progress / static_cast<double>(max);
	const uint8_t cur = static_cast<uint8_t>(abs(ratio * _length));

	uint8_t col;
	if (colored) {
		// determing color
		if (ratio < 0.33) {
			//red
			uint8_t rest = static_cast<uint8_t>(abs((ratio) * 10.0));
			col = 0x27 + rest;
			// 0.67 - 0.33 = 0.34 (0.33)
		}
		else if (ratio < 0.67) {
			// yellow
			uint8_t rest = static_cast<uint8_t>(abs((ratio - 0.33) * 10.0));
			col = 0x2A + rest;
		}
		else {
			// green
			uint8_t rest = static_cast<uint8_t>(abs((ratio - 0.67) * 10.0));
			col = 0x2D + rest;
		}
	}
	else {
		col = _col;
	}

	for (int i = 0; i < cur; i++) {
		_term.write(219, col, bgCol);
	}
	for (int i = cur; i < _length; i++) {
		_term.write(176, _col, bgCol);
	}

	if (showProgress) {
		char buf[bufSize];

		snprintf(buf, bufSize, "%3.0f%%", 100.0 * ratio);
		_term.writeXY(x + _length + 1, y, buf, _col, bgCol);
	}
}


void TuiTerminal::_drawBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str, const uint8_t tc, const uint8_t mc, const uint8_t bc)
{
	if (r.h < 3) {
		throw std::invalid_argument("height must be 3 at least");
	}

	// Top
	_term.gotoXY(r.x, r.y);
	_term.write(tc, col, bgCol);
	_drawHBorder(r.x + 1, r.y, r.w, col, bgCol, 205);
	// Middle
	for (uint8_t i = 1; i < r.h - 1; i++) {
		_term.gotoXY(r.x, r.y + i);
		_term.write(mc, col, bgCol);
	}

	// at the moment only on the 2nd row other are just "padding"
	_term.writeXY(r.x + 1, r.y + 1, tui::alignString(str, r.w - 2 - str.size(), ' '), col, bgCol);

	// Bottom
	_term.gotoXY(r.x, r.y + r.h - 1);
	_term.write(bc, col, bgCol);
	_drawHBorder(r.x + 1, r.y + r.h - 1, r.w, col, bgCol, 205);

}

void TuiTerminal::_drawLeftBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str)
{
	_drawBorder(r, col, bgCol, str, 201, 186, 200);
}

void TuiTerminal::_drawMiddleBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str)
{
	_drawBorder(r, col, bgCol, str, 209, 179, 207);
}

void TuiTerminal::_drawRightBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str)
{
	_drawMiddleBorder(r, col, bgCol, str);
	
	uint8_t xw = r.x + r.w - 1;
	_term.gotoXY(xw, r.y);
	_term.write(187, col, bgCol);
	for (uint8_t i = 1; i < r.h - 1; i++) {
		_term.gotoXY(xw, r.y + i);
		_term.write(186, col, bgCol);
	}
	_term.gotoXY(xw, r.y + r.h - 1);
	_term.write(188, col, bgCol);
}

void TuiTerminal::_drawHBorder(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t col, const uint8_t bgCol, const uint8_t c) noexcept
{
	_term.gotoXY(x, y);
	for (uint8_t i = 2; i < width; i++) {
		_term.write(c, col, bgCol);
	}
}

void TuiTerminal::_unitShadowColor(uint8_t& col)
{
	if (col < 8) {
		col = 0;
	}
	else if (col < 16) {
		// in the 16 colors range
		col &= 0x7;
	}
	else if (col < 24) {
		// dark gray scale
		col = 16;
	}
	else if (col < 32) {
		col -= 8;
	}
	else if (col < 104) {
		// light color
		col += 103;
	}
	else {
		// dark color
		// didn't exploit a mapping
		col = 0;
	}
}

void TuiTerminal::_shadowColor(VgaTerminal::terminalChar_t& tc)
{
	_unitShadowColor(tc.col);
	_unitShadowColor(tc.bgCol);
}

void TuiTerminal::_drawShadow(const SDL_Rect& r) noexcept
{
	_term.gotoXY(r.x + 2, r.y + r.h);
	for (int i = 0; i < r.w; i++) {
		auto tc = _term.at(_term.getX(), _term.getY());
		_shadowColor(tc);
		_term.write(tc);
	}

	for (int i = 1; i < r.h; i++) {
		_term.gotoXY(r.x + r.w, r.y + i);
		for (int j = 0; j < 2; j++) {
			auto tc = _term.at(_term.getX(), _term.getY());
			_shadowColor(tc);
			_term.write(tc);
		}
	}
}

void TuiTerminal::_drawFooter() noexcept
{
	_term.writeXY(0, 24, " D", 4, 15);
	_term.write("OS TUI Emulation...", 0, 15);
	for (int i = _term.getX(); i < _term.getMode().tw; i++) {
		_term.write(footer_tc.c, footer_tc.col, footer_tc.bgCol);
	}
}
