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
        static_cast <uint8_t>(8),     // cw
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

VgaTerminal::~VgaTerminal()
{
    if (NULL != _cursorTimer) {
        SDL_RemoveTimer(_cursorTimer);
    }
}

VgaTerminal::VgaTerminal(const VgaTerminal& t) : Window(t)
{
    
    throw std::runtime_error("not implemented");
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
        p.colors[i].r = RESIZE_VGA_PALETTE(mode.palette[i3 + 2]);
        p.colors[i].g = RESIZE_VGA_PALETTE(mode.palette[i3 + 1]);
        p.colors[i].b = RESIZE_VGA_PALETTE(mode.palette[i3 + 0]);
        p.colors[i].a = 255;
    }

    _pGrid = std::make_unique<terminalChar_t[]>(static_cast<uint64_t>(mode.tw) * mode.th);
    if (!_pGrid) {
        throw std::runtime_error("unable to alloc _pGrid");
    }

    if(SDL_WasInit(SDL_INIT_TIMER) == SDL_INIT_TIMER) {
        if (SDL_AddTimer(cursor_time, _timerCallBack, this) == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "[% s] % s: unable to install cursor callback.", typeid(*this).name(), __func__);
        }
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

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @return VgaTerminal::terminalChar_t defaultNullChar if out of screen
 */
VgaTerminal::terminalChar_t VgaTerminal::at(const uint8_t x, const uint8_t y) const
{
    return (x >= mode.tw || y >= mode.th)
        ? defaultNullChar
        : _pGrid[static_cast<size_t>(y) * mode.tw + x]
    ;
}

void VgaTerminal::render(const bool force)
{
    // TODO:
    // for performances this if should not be done
    // and slice the for loop without the curX,Y pos
    // like: (j =0; j< _curY), (j=_curY+1; j<mode.th) => the i loop as usual
    //       j=curY, (i=0; <_curX),/(i=_curX+1; i<mode.tw)
    //       j=curY, i=curX
    //
    //       at that point the loops is a general function accepting the 4 vaues for the range
    //       special case only for the cursor, 
    //       and can just ignore the rendered flag
    //

    if (!force && (SDL_GetWindowFlags(getWindow()) & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN) {
        return;
    }

    // force to render the cursor everytime
    int icur = static_cast<int>(_curY) * mode.tw + _curX;
    _pGrid[icur].rendered = false;

    for (register int j = 0; j < mode.th; j++) {
        register int j2 = j * mode.tw;
        register int jch = j * mode.ch;
        
        for (register int i = 0; i < mode.tw; i++) {
            register int i2 = j2 + i;

            if (!force && _pGrid[i2].rendered) {
                continue;
            }
        
            SDL_Point p = { i * mode.cw, jch };
            uint8_t col, bgCol, c;
            c = _pGrid[i2].c;
            col = _pGrid[i2].col;
            bgCol = _pGrid[i2].bgCol;

            // cursor position
            if ((_curY == j) && (_curX == i)
                && (showCursor && _cursonOn)) {
                
                if (c != 0) {
                    // invert col and bgCol
                    col = _pGrid[i2].bgCol;
                    bgCol = _pGrid[i2].col;
                }
                else {
                    // only the cursor
                    bgCol = 0;
                    col = curDefaultCol;
                    c = cur_shape;
                }
            }
            else {
                _pGrid[i2].rendered = true;
            }

            renderChar(p, col, bgCol, c);
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

void VgaTerminal::moveCursorLeft() noexcept
{
    if (_curX > 0) {
        //gotoXY(_curX - 1, _curY);
        --_curX;
    }
    else if(_curY > 0) {
        --_curY;
        _curX = mode.tw - 1;
    }
    else {
        // alredy in 0,0 ... what should i do? :)
    }
    
}

void VgaTerminal::moveCursorRight() noexcept
{
    if (_curX < mode.tw - 1) {
        ++_curX;
    }
    else if (_curY < mode.th - 1) {
        _curY++;
        _curX = 0;
    }
    else {
        //already at the max
    }
}

void VgaTerminal::moveCursorUp() noexcept
{
    if (_curY > 0) {
        --_curY;
    }
    else {
        // no scroll yet
    }
}

void VgaTerminal::moveCursorDown() noexcept
{
    if (_curY < mode.th - 1) {
        ++_curY;
    }
}

uint32_t VgaTerminal::_timerCallBack(uint32_t interval, void* param)
{
    SDL_Event event;
    SDL_UserEvent userevent;
    VgaTerminal* that = (VgaTerminal*)param;
    
    that->_cursonOn = !that->_cursonOn;
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return interval;
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
    for (register int j = 1, j2 = mode.tw, jj2 = 0;
        j < mode.th;
        j++, j2 += mode.tw, jj2 += mode.tw)
    {
        for (register int i = 0, i2 = j2, ii2 = jj2;
            i < mode.tw;
            i++, i2++, ii2++)
        {
            if (_pGrid[i2].rendered && _pGrid[ii2] == _pGrid[i2]) {
                continue;
            }

            _pGrid[ii2] = _pGrid[i2];
            _pGrid[ii2].rendered = false;
        }
    }
    
    // clear line
    register int j2 = (mode.th - 1) * mode.tw;
    for (register int i = 0; i < mode.tw; i++) {
        _pGrid[static_cast<uint64_t>(i) + j2] = defaultNullChar;
    }
}


