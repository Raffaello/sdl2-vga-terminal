#include <VgaTerminal.hpp>
#include <typeinfo>
#include <SDL2/SDL_assert.h>
#include <stdexcept>
#include <vgafonts.h>
#include <vgapalette.h>

constexpr auto VGA_TERMINAL_NUM_CHARS = VGA_FONT_CHARS;

template<typename T> 
constexpr auto RESIZE_VGA_PALETTE(T x) { return (x * 255 / 0x3f); }

const VgaTerminal::videoMode_t VgaTerminal::mode3 = {
        static_cast <uint8_t>(0x003), // mode
        static_cast <uint8_t>(80),    // tw
        static_cast <uint8_t>(25),    // th
        static_cast <uint8_t>(9),     // cw
        static_cast <uint8_t>(VGA_FONT_SIZE_16),    // ch
        //static_cast <uint8_t>(16)     // fs
        vgafont16,
        PALETTE_3_COLORS,
        palette3,
};

bool VgaTerminal::terminalChar_t::operator==(const terminalChar_t& o) const
{
    return rendered == o.rendered
        && c == o.c
        && col == o.col
        && bgCol == o.bgCol;
}

VgaTerminal::VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags) : 
    VgaTerminal(title, mode3.tw * mode3.cw, mode3.th * mode3.ch, winFlags, drvIndex, renFlags)
{
}

VgaTerminal::VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags) :
    Window(title, width, height, winFlags, drvIndex, renFlags)
{
    mode = mode3;
    if (SDL_RenderSetLogicalSize(getRenderer(), mode.tw * mode.cw, mode.th * mode.ch) < 0) {
        throw std::runtime_error("unable to set logical rendering");
    }

    p.ncolors = mode.numColors;
    pCol = std::make_unique<SDL_Color[]>(p.ncolors);
    p.colors = pCol.get();
    for (int i = 0, i3 = 0; i < p.ncolors; i++, i3+=3)
    {
        p.colors[i].r = RESIZE_VGA_PALETTE(palette3[i3 + 0]);
        p.colors[i].g = RESIZE_VGA_PALETTE(palette3[i3 + 1]);
        p.colors[i].b = RESIZE_VGA_PALETTE(palette3[i3 + 2]);
        p.colors[i].a = 255;
    }

    _pGrid = std::make_unique<terminalChar_t[]>(static_cast<uint64_t>(mode.tw) * mode.th);
    if (!_pGrid) {
        throw std::runtime_error("unable to alloc _pGrid");
    }
}

void VgaTerminal::renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c)
{
    register const uint16_t offs = static_cast<uint8_t>(c) * mode.ch;
    register const uint8_t lsz = 8;
    register const int dstx = dst.x + lsz;
    const SDL_Color col_ = p.colors[col];
    const SDL_Color bgCol_ = p.colors[bgCol];
    for (register uint8_t y = 0; y < mode.ch; y++)
    {
        const std::bitset<lsz> line(mode.font[offs + y]);
        register const int dsty = dst.y + y;
       
        // *** render line ***
        for (register uint8_t x = 0; x < lsz; x++) {
            const SDL_Color col__ = line[x] == 1 ? col_ : bgCol_;
            SDL_SetRenderDrawColor(getRenderer(), col__.r, col__.g, col__.b, col__.a);
            SDL_RenderDrawPoint(getRenderer(), dstx - x, dsty);
        }
     
        SDL_SetRenderDrawColor(getRenderer(), bgCol_.r, bgCol_.g, bgCol_.b, bgCol_.a);
        SDL_RenderDrawPoint(getRenderer(), dst.x, dsty);
        // end *** render line ***
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

void VgaTerminal::gotoXY(const position_t &position)
{
    gotoXY(position.first, position.second);
}

VgaTerminal::position_t VgaTerminal::getXY() const
{
    return position_t(_curX, _curY);
}

uint8_t VgaTerminal::getX() const
{
    return _curX;
}

uint8_t VgaTerminal::getY() const
{
    return _curY;
}

void VgaTerminal::write(const char c, const uint8_t col, const uint8_t bgCol)
{
    int pos = _curX + _curY * mode.tw;
    _pGrid[pos].c = c;
    _pGrid[pos].col = col;
    _pGrid[pos].bgCol = bgCol; 
    _pGrid[pos].rendered = false;
    //_pGrid[pos] = {static_cast<uint8_t>(c), col, bgCol, false};
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

VgaTerminal::terminalChar_t VgaTerminal::at(const uint8_t x, const uint8_t y) const
{
    return (x >= mode.tw || y >= mode.th)
        ? defaultNullChar
        : _pGrid[static_cast<size_t>(y) * mode.tw + x]
    ;
}

void VgaTerminal::render()
{
    for (register int j = 0; j < mode.th; j++) {
        register int j2 = j * mode.tw;
        register int jch = j * mode.ch;
        
        for (register int i = 0; i < mode.tw; i++) {
            register int i2 = j2 + i;
            if (_pGrid[i2].rendered) {
                continue;
            }
            
            SDL_Point p = { i * mode.cw, jch };
            renderChar(p, _pGrid[i2].col, _pGrid[i2].bgCol, _pGrid[i2].c);
            _pGrid[i2].rendered = true;
        }
    }

    renderPresent();
}

void VgaTerminal::clearGrid()
{
    // TODO: can just use memcpy to repeatealy copy the null char in the grid?
    //       only if is all zeros.
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
    if (++_curX >= mode.tw)
    {
        _curX = 0;
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
            if (_pGrid[i2].rendered && _pGrid[ii2] == _pGrid[i2]) {
                continue;
            }

            _pGrid[ii2] = _pGrid[i2];
            _pGrid[ii2].rendered = false;
        }
    }
    
    int j2 = (mode.th - 1) * mode.tw;
    for (int i = 0; i < mode.tw; i++) {
        _pGrid[static_cast<uint64_t>(i) + j2] = defaultNullChar;
    }
}


