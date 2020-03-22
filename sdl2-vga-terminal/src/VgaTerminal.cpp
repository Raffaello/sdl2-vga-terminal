#include <VgaTerminal.hpp>
#include <typeinfo>
#include <SDL2/SDL_assert.h>
#include <bitset>
#include <stdexcept>
#include <vgafonts.h>
#include <vgapalette.h>

// TODO: find a better parametrized way to choose among the 3 palettes.
constexpr auto VGA_TERMINAL_NUM_COLORS = PALETTE_3_COLORS;
// TODO: find a better parametrized way to choose among the vga fonts too.
constexpr auto VGA_TERMNIAL_NUM_CHARS = VGA_FONT_CHARS;

template<typename T> 
constexpr auto RESIZE_VGA_PALETTE(T x) { return (x * 255 / 0x3f); }

const VgaTerminal::videoMode_t VgaTerminal::mode3 = {
        static_cast <uint8_t>(0x003), // mode
        static_cast <uint16_t>(720),  // sw (tw * cw) [redundant]
        static_cast <uint16_t>(400),  // sh (th * ch) [redundant]
        static_cast <uint8_t>(80),    // tw
        static_cast <uint8_t>(25),    // th
        static_cast <uint8_t>(9),     // cw
        static_cast <uint8_t>(16)     // ch
};

VgaTerminal::VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags) : 
    VgaTerminal(title, mode3.sw, mode3.sh, winFlags, drvIndex, renFlags)
{
}

VgaTerminal::VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags) :
    Window(title, width, height, winFlags, drvIndex, renFlags)
{
    mode = mode3;
    if (SDL_RenderSetLogicalSize(getRenderer(), mode.sw, mode.sh) < 0) {
        throw std::runtime_error("unable to set logical rendering");
    }

    p.ncolors = VGA_TERMINAL_NUM_COLORS;
    pCol = std::make_unique<SDL_Color[]>(VGA_TERMINAL_NUM_COLORS);
    p.colors = pCol.get();
    for (int i = 0; i < VGA_TERMINAL_NUM_COLORS; i++)
    {
        p.colors[i].r = RESIZE_VGA_PALETTE(palette3[i][0]);
        p.colors[i].g = RESIZE_VGA_PALETTE(palette3[i][1]);
        p.colors[i].b = RESIZE_VGA_PALETTE(palette3[i][2]);
        p.colors[i].a = 255;
    }

    _pGrid = std::make_unique<terminalChar_t[]>(static_cast<uint64_t>(mode.tw) * mode.th);
    if (!_pGrid) {
        throw std::runtime_error("unable to alloc _pGrid");
    }
}

void VgaTerminal::renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c)
{
    register const uint16_t offs = static_cast<uint8_t>(c) << 4;
    register const uint8_t lsz = 8;
    for (register uint8_t y = 0; y < mode.ch; y++)
    {
        register const uint16_t yw = y * mode.cw;
        // TODO: vgafont16 should be parametrized
        const std::bitset<lsz> line(vgafont16[offs + y]);
        for (register uint8_t x = 0; x < lsz; x++) {
            const uint8_t col_ = line[x] == 1 ? col : bgCol;
            // TODO: Decouple specific SDL implementation.
            SDL_SetRenderDrawColor(getRenderer(), p.colors[col_].r, p.colors[col_].g, p.colors[col_].b, p.colors[col_].a);
            SDL_RenderDrawPoint(getRenderer(), dst.x + lsz - x, dst.y + y);
        }
    }
}

void VgaTerminal::gotoXY(const uint8_t x, const uint8_t y)
{
    if ((x < mode.tw) && (y < mode.th)) {
        _curX = x;
        _curY = y;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[%s] %s: outside the grid. Ignoring.", typeid(*this).name(), __func__);
    }
}

void VgaTerminal::write(const char c, const uint8_t col, const uint8_t bgCol)
{
    register int pos = _curX + _curY * mode.tw;
    _pGrid[pos].c = c;
    _pGrid[pos].col = col;
    _pGrid[pos].bgCol = bgCol; 
    _pGrid[pos].rendered = false;
    incrementCursorPosition();
}

void VgaTerminal::write(const std::string &str, const uint8_t col, const uint8_t bgCol)
{
    register size_t sz = str.size();
    for (register size_t i = 0; i < sz; i++) {
        write(str.at(i), col, bgCol);
    }
}

void VgaTerminal::writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol)
{
    gotoXY(x, y);
    write(str, col, bgCol);
}

void VgaTerminal::render()
{
    for (register int j = 0; j < mode.th; j++) {
        register int j2 = j * mode.tw;
        for (register int i = 0; i < mode.tw; i++) {
            register int i2 = j2 + i;
            if (_pGrid[i2].rendered) {
                continue;
            }
            SDL_Point p = { i * mode.cw, j * mode.ch };
            renderChar(p, _pGrid[i2].col, _pGrid[i2].bgCol, _pGrid[i2].c);
            _pGrid[i2].rendered = true;
        }
    }

    renderPresent();
}

void VgaTerminal::clearGrid()
{
    // TODO: can just use memcpy to repeatealy copy the null char in the grid?
    for (int j = 0; j < mode.th; j++) {
        int j2 = j * mode.tw;
        for (int i = 0; i < mode.tw; i++) {
            int i2 = i + j2;
            _pGrid[i2] = defaultNullChar;
        }
    }

    gotoXY(0, 0);
}

void VgaTerminal::incrementCursorPosition()
{
    //_curX++;
    if (++_curX >= mode.tw)
    {
        _curX = 0;
        //_curY++;
        if (++_curY >= mode.th)
        {
            _curY = mode.th - 1;
            scrollDownGrid();
        }
    }
}

void VgaTerminal::scrollDownGrid()
{
    for (int j = 1; j < mode.th; j++) {
        register int j2 = j * mode.tw;
        register int jj2 = j2 - mode.tw;
        for (int i = 0; i < mode.tw; i++) {
            register int i2 = i + j2;
            register int ii2 = i + jj2;
            if (_pGrid[i2].rendered
                && _pGrid[ii2].c == _pGrid[i2].c
                &&_pGrid[ii2].bgCol == _pGrid[i2].bgCol  
                && _pGrid[ii2].col == _pGrid[i2].col) {
                continue;
            }

            _pGrid[ii2] = _pGrid[i2];
            _pGrid[ii2].rendered = false;
        }
    }
    
    int j2 = (mode.th - 1) * mode.tw;
    for (int i = 0; i < mode.tw; i++) {
        _pGrid[static_cast<uint64_t>(i) + j2] = defaultNullChar;
        //_pGrid[i2].c = 0;
        //_pGrid[i2].col = 0;
        //_pGrid[i2].bgCol = _bgCol;
        //_pGrid[i2].rendered = false;
    }
}
