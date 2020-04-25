#include "VgaTerminal.hpp"
#include <typeinfo>
#include <stdexcept>
#include "vgafonts.h"
#include "vgapalette.h"
#include <version.h>

constexpr auto VGA_TERMINAL_NUM_CHARS = VGA_FONT_CHARS;

template<typename T> 
constexpr auto RESIZE_VGA_PALETTE(T x) { return (x * 255 / 0x3F); }

const VgaTerminal::videoMode_t VgaTerminal::mode3 = {
        static_cast <uint8_t>(0x003), // mode
        static_cast <uint8_t>(80),    // tw
        static_cast <uint8_t>(25),    // th
        static_cast <uint8_t>(8),     // cw
        static_cast <uint8_t>(VGA_FONT_SIZE_16), // ch
        PALETTE_3_COLORS,
        vgafont16,
        palette3,
};

bool VgaTerminal::_terminalChar_t::operator==(const _terminalChar_t& o) const
{
    return rendered == o.rendered
        && c == o.c
        && col == o.col
        && bgCol == o.bgCol;
}

const std::string VgaTerminal::getVersion()
{
    return std::string(VGA_TERMINAL_VERSION);
}

VgaTerminal::~VgaTerminal()
{
    if (_cursorTimerId != 0) {
        SDL_RemoveTimer(_cursorTimerId);
    }
}

VgaTerminal::VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags) :
    VgaTerminal(title, mode3.tw * mode3.cw, mode3.th * mode3.ch, winFlags, drvIndex, renFlags)
{
}

VgaTerminal::VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags) :
    Window(title, width, height, winFlags, drvIndex, renFlags),
    _viewPortX(0), _viewPortY(0)
{
    mode = mode3;
    _viewPortWidth = mode.tw, _viewPortHeight = mode.th;

    if (SDL_RenderSetLogicalSize(getRenderer(), mode.tw * mode.cw, mode.th * mode.ch) < 0) {
        throw std::runtime_error(std::string("unable to set logical rendering. Error: ") + SDL_GetError());
    }

    _pal.ncolors = mode.numColors;
    pCol = std::make_unique<SDL_Color[]>(_pal.ncolors);
    _pal.colors = pCol.get();
    for (int i = 0, i3 = 0; i < _pal.ncolors; i++, i3+=3)
    {
        _pal.colors[i].r = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 0]));
        _pal.colors[i].g = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 1]));
        _pal.colors[i].b = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 2]));
        _pal.colors[i].a = 255;
    }

    _pGrid = std::make_unique<_terminalChar_t[]>(static_cast<uint64_t>(mode.tw) * mode.th);
    if (!_pGrid) {
        throw std::runtime_error("unable to alloc _pGrid");
    }
    
    if((SDL_WasInit(SDL_INIT_TIMER) == SDL_INIT_TIMER) && (SDL_WasInit(SDL_INIT_EVENTS) == SDL_INIT_EVENTS)) {
        _cursorTimerId = SDL_AddTimer(cursor_time, _timerCallBack, this);
        if (_cursorTimerId == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "[%s] %s: unable to install cursor callback.", typeid(*this).name(), __func__);
        }
    }
    else {
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "[%s] %s: TIMER or EVENTS not inited.", typeid(*this).name(), __func__);
    }
}

void VgaTerminal::_renderCharLine(const std::bitset<8> line, const int dstx, const int dsty, uint8_t col, uint8_t bgCol)
{
    constexpr auto lsz = 8;

    // *** render line ***
    SDL_Point points[lsz];
    uint8_t fgi = 0, bgi = lsz;
    for (uint8_t x = 0; x < lsz; x++) {
        if (line.test(x)) points[fgi++] = { dstx - x, dsty };
        else points[--bgi] = { dstx - x, dsty };
    }

    SDL_SetRenderDrawColor(getRenderer(), _pal.colors[col].r, _pal.colors[col].g, _pal.colors[col].b, _pal.colors[col].a);
    SDL_RenderDrawPoints(getRenderer(), points, fgi);
    SDL_SetRenderDrawColor(getRenderer(), _pal.colors[bgCol].r, _pal.colors[bgCol].g, _pal.colors[bgCol].b, _pal.colors[bgCol].a);
    SDL_RenderDrawPoints(getRenderer(), &points[fgi], lsz - bgi);

}
void VgaTerminal::_renderFontChar(const SDL_Point& dst, _terminalChar_t& tc)
{
    uint8_t* font = &mode.font[static_cast<uint16_t>(tc.c) * mode.ch];
    constexpr uint8_t lsz = sizeof(uint8_t) * 8;
    const int dstx = dst.x + lsz;

    for (uint8_t y = 0; y < mode.ch; y++)
    {
        _renderCharLine(font[y], dstx, dst.y + y, tc.col, tc.bgCol);
    }

    tc.rendered = true;
}

void VgaTerminal::_renderCursor(const SDL_Point& dst, _terminalChar_t& tc)
{
    if (showCursor && _drawCursor) {
        const uint8_t col = tc.col == tc.bgCol ? 
            cursorDefaultCol :
            tc.col
        ;
            
        constexpr uint8_t lsz = sizeof(uint8_t) * 8;
        const int dstx = dst.x + lsz;
        const SDL_Color col_ = _pal.colors[col];
        const SDL_Color bgCol_ = _pal.colors[tc.bgCol];
        const uint8_t* font = &mode.font[static_cast<uint16_t>(tc.c) * mode.ch];
        const uint8_t* cursorFont = &vgafont_cursors16[static_cast<int>(cursor_mode) * mode.ch];

        for (uint8_t y = 0; y < mode.ch; y++)
        {
            _renderCharLine(font[y] ^ cursorFont[y], dstx, dst.y + y, col, tc.bgCol);
        }
    }
    else {
        _renderFontChar(dst, tc);
    }
}

void VgaTerminal::gotoXY(const uint8_t x, const uint8_t y) noexcept
{
    if ((x < _viewPortWidth) && (y < _viewPortHeight)) {
        _curX = _viewPortX + x;
        _curY = _viewPortY + y;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[%s] %s: outside the grid. Ignoring.", typeid(*this).name(), __func__);
    }
}

void VgaTerminal::gotoXY(const position_t &position) noexcept
{
    gotoXY(position.first, position.second);
}

VgaTerminal::position_t VgaTerminal::getXY() const noexcept
{
    return position_t(getX(), getY());
}

uint8_t VgaTerminal::getX() const noexcept
{
    return _curX - _viewPortX;
}

uint8_t VgaTerminal::getY() const noexcept
{
    return _curY - _viewPortY;
}

void VgaTerminal::write(const char c, const uint8_t col, const uint8_t bgCol) noexcept
{
    int pos = _curX + _curY * mode.tw;
    _pGrid[pos].c = c;
    _pGrid[pos].col = col;
    _pGrid[pos].bgCol = bgCol; 
    _pGrid[pos].rendered = false;
    _incrementCursorPosition();
}

void VgaTerminal::write(const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept
{
    size_t sz = str.size();
    for (size_t i = 0; i < sz; i++) {
        write(str[i], col, bgCol);
    }
}

void VgaTerminal::writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept
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
VgaTerminal::terminalChar_t VgaTerminal::at(const uint8_t x, const uint8_t y) const noexcept
{
    if (x >= _viewPortWidth || y >= _viewPortHeight) {
        return defaultNullChar;
    }

    _terminalChar_t _tc = _pGrid[(static_cast<size_t>(y) + _viewPortY) * mode.tw + x + _viewPortX];
    terminalChar_t tc;
    tc.c = _tc.c;
    tc.col = _tc.col;
    tc.bgCol = _tc.bgCol;
 
    return tc;
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

    for (int j = 0; j < mode.th; j++) {
        int j2 = j * mode.tw;
        int jch = j * mode.ch;

        for (int i = 0; i < mode.tw; i++) {
            int i2 = j2 + i;

            if (!force && _pGrid[i2].rendered) {
                continue;
            }

            SDL_Point p = { i * mode.cw, jch };
            // cursor position
            if ((_curY == j) && (_curX == i)) {
                _renderCursor(p, _pGrid[i2]);
            }
            else {
                //_pGrid[i2].rendered = true;
                _renderFontChar(p, _pGrid[i2]);
            }
        }
    }

    renderPresent();
}

void VgaTerminal::clear() noexcept
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

    gotoXY(_viewPortX, _viewPortY);
}

void VgaTerminal::moveCursorLeft() noexcept
{
    if (_curX > _viewPortX) {
        //gotoXY(_curX - 1, _curY);
        --_curX;
    }
    else if(_curY > _viewPortY) {
        --_curY;
        _curX =_viewPortX + _viewPortWidth - 1;
    }
    else {
        // alredy in 0,0 ... what should i do? :)
    }
    
}

void VgaTerminal::moveCursorRight() noexcept
{
    _incrementCursorPosition(false);
}

void VgaTerminal::moveCursorUp() noexcept
{
    if (_curY > _viewPortY) {
        --_curY;
    }
    else {
        // no scroll yet
    }
}

void VgaTerminal::moveCursorDown() noexcept
{
    if (_curY < _viewPortY + _viewPortHeight - 1) {
        ++_curY;
    }
}

void VgaTerminal::newLine() noexcept
{
    _curX = _viewPortX;
    auto oldY = _curY;
    moveCursorDown();
    // last line
    if ((oldY == _curY) && (autoScroll)) {
        _scrollDownGrid();
    }
}

bool VgaTerminal::setViewPort(const position_t& viewport, const uint8_t width, const uint8_t height) noexcept
{
    auto& [x, y] = viewport;
    return setViewPort(x, y, width, height);
}

bool VgaTerminal::setViewPort(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height) noexcept
{
    if ((x+width > mode.tw) || (y+height > mode.th)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] %s: viewport larger than terminal.", typeid(*this).name(), __func__);
        return false;
    }

    if ((width == 0) || (height == 0)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] %s: viewport too small.", typeid(*this).name(), __func__);
        return false;
    }

    _viewPortX = x;
    _viewPortY = y;
    _viewPortWidth = width;
    _viewPortHeight = height;
    _curX = _viewPortX;
    _curY = _viewPortY;

    return true;
}

bool VgaTerminal::setViewPort(const SDL_Rect& r) noexcept
{
    uint8_t x = static_cast<uint8_t>(r.x), y = static_cast<uint8_t>(r.y),
        w = static_cast<uint8_t>(r.w), h = static_cast<uint8_t>(r.h);

    return setViewPort(x, y, w, h);
}

SDL_Rect VgaTerminal::getViewport() const noexcept
{
    //return { _viewPortX, _viewPortY, _viewPortWidth, _viewPortHeight };
    SDL_Rect r;
    r.x = _viewPortX;
    r.y = _viewPortY;
    r.w = _viewPortWidth;
    r.h = _viewPortHeight;
    return r;
}

void VgaTerminal::resetViewport() noexcept
{
    setViewPort(0, 0, mode.tw, mode.th);
}

uint32_t VgaTerminal::_timerCallBack(uint32_t interval, void* param)
{
    // TODO review the user event
    SDL_Event event;
    SDL_UserEvent userevent;
    VgaTerminal* that = reinterpret_cast<VgaTerminal*>(param);
    
    that->_drawCursor = !that->_drawCursor;
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return interval;
}

void VgaTerminal::_incrementCursorPosition(bool increment) noexcept
{
    if (_curX < _viewPortX + _viewPortWidth - 1) {
        ++_curX;
    }
    else if (_curY < _viewPortY + _viewPortHeight - 1) {
        _curY++;
        _curX = _viewPortX;
    }
    else if ((increment) && (autoScroll)) {
        //already at the max
        _curX = _viewPortX;
        _scrollDownGrid();
    }
}

void VgaTerminal::_scrollDownGrid() noexcept
{
    // todo can be used as private fields instead of local vars.
    auto vh = _viewPortY + _viewPortHeight;
    auto vw = _viewPortWidth + _viewPortWidth;

    for (int j = _viewPortY + 1; j < vh ; j++)
    {
        int j2 = j * mode.tw;
        int jj2 = j2 - mode.tw;
        
        for (int i = _viewPortX; i < vw; i++)
        {
            int i2 = i + j2;
            int ii2 = i + jj2;
            if (_pGrid[i2].rendered && _pGrid[ii2] == _pGrid[i2]) {
                continue;
            }

            _pGrid[ii2] = _pGrid[i2];
            _pGrid[ii2].rendered = false;
        }
    }

    // clear line
    //int j2 = (mode.th - 1) * mode.tw;
    int j2 = (vh - 1) * mode.tw + _viewPortX;
    for (int i = 0; i < _viewPortWidth; i++) {
        _pGrid[static_cast<uint64_t>(i) + j2] = defaultNullChar;
    }
}

const VgaTerminal::videoMode_t VgaTerminal::getMode() const noexcept
{
    return mode;
}
