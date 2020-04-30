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
        vgafont_cursors16,
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

VgaTerminal::VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags)
    : VgaTerminal(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, winFlags, drvIndex, renFlags)
{
}

VgaTerminal::VgaTerminal(const std::string& title, const int x, const int y, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags)
    : Window(title, x, y, width, height, winFlags, drvIndex, renFlags),
    mode(mode3), _viewPortX(0), _viewPortY(0)
{
    //mode = mode3;
    _viewPortWidth = mode.tw, _viewPortHeight = mode.th;

    if (SDL_RenderSetLogicalSize(getRenderer(), mode.tw * mode.cw, mode.th * mode.ch) < 0) {
        throw std::runtime_error(std::string("unable to set logical rendering. Error: ") + SDL_GetError());
    }

    _pal.ncolors = mode.numColors;
    pCol = std::make_unique<SDL_Color[]>(_pal.ncolors);
    _pal.colors = pCol.get();
    for (int i = 0, i3 = 0; i < _pal.ncolors; i++, i3 += 3)
    {
        _pal.colors[i].r = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 0]));
        _pal.colors[i].g = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 1]));
        _pal.colors[i].b = static_cast<uint8_t>(RESIZE_VGA_PALETTE(mode.palette[i3 + 2]));
        _pal.colors[i].a = SDL_ALPHA_OPAQUE;
    }

    _pGrid = std::make_unique<_terminalChar_t[]>(static_cast<uint64_t>(mode.tw) * mode.th);

    if ((SDL_WasInit(SDL_INIT_TIMER) == SDL_INIT_TIMER) && (SDL_WasInit(SDL_INIT_EVENTS) == SDL_INIT_EVENTS)) {
        _cursorTimerId = SDL_AddTimer(cursor_time, _timerCallBackWrapper, this);
        if (_cursorTimerId == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "[%s] %s: unable to add cursor callback. Error: %s",
                typeid(*this).name(), __func__, SDL_GetError());
        }
    }
    else {
        SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "[%s] %s: TIMER or EVENTS not inited.", typeid(*this).name(), __func__);
    }
}

void VgaTerminal::_renderFontChar(const SDL_Point& dst, _terminalChar_t& tc)
{
    uint8_t* font = &mode.font[static_cast<uint16_t>(tc.c) * mode.ch];
    const int dstx = dst.x + mode.cw;

    for (uint8_t y = 0; y < mode.ch; y++) {
        _renderCharLine(font[y], dstx, dst.y + y, tc.col, tc.bgCol);
    }

    tc.rendered = true;
}

void VgaTerminal::_renderCharLine(const std::bitset<8>& line, const int dstx, const int dsty, const uint8_t col, const uint8_t bgCol)
{
    // TODO lsz is mode.cw,
    //      start make sense spliting to a VgaTerminalRender, all the 
    //      rendering functions.
    constexpr auto lsz = 8;

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

void VgaTerminal::_renderCursor(const SDL_Point& dst, _terminalChar_t& tc)
{
    const uint8_t col = tc.col == tc.bgCol ? 
        cursorDefaultCol :
        tc.col
    ;
            
    const int dstx = dst.x + mode.cw;
    const uint8_t* font = &mode.font[static_cast<int>(tc.c) * mode.ch];
    const uint8_t* cursorFont = &mode.cursors[static_cast<int>(cursor_mode) * mode.ch];

    for (uint8_t y = 0; y < mode.ch; y++) {
        _renderCharLine(font[y] ^ cursorFont[y], dstx, dst.y + y, col, tc.bgCol);
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
    return std::make_pair(getX(), getY());
}

uint8_t VgaTerminal::getX() const noexcept
{
    return _curX - _viewPortX;
}

uint8_t VgaTerminal::getY() const noexcept
{
    return _curY - _viewPortY;
}

void VgaTerminal::write(const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept
{
    // todo make sense to have a _getCursorPosition method.
    int pos = _curX + _curY * mode.tw;
    _terminalChar_t tc;
    tc.bgCol = bgCol, tc.c = c, tc.col = col, tc.rendered = false;
    {
        std::lock_guard lck(_pGridMutex);
        _pGrid[pos] = tc;
    }
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
VgaTerminal::terminalChar_t VgaTerminal::at(const uint8_t x, const uint8_t y) noexcept
{
    if (x >= _viewPortWidth || y >= _viewPortHeight) {
        return _defaultNullChar;
    }
   
    _terminalChar_t _tc;
    {
        std::lock_guard lck(_pGridMutex);
        _tc = _pGrid[(static_cast<size_t>(y) + _viewPortY) * mode.tw + x + _viewPortX];
    }
    terminalChar_t tc;
    tc.c = _tc.c,
    tc.col = _tc.col,
    tc.bgCol = _tc.bgCol;
 
    return tc;
}

void VgaTerminal::_renderGridPartialY(const uint8_t y1, const uint8_t y2, const bool force)
{
    for (int j = y1, j2= y1 * mode.tw, jch = y1 * mode.ch;
        j < y2;
        j++, j2 += mode.tw, jch += mode.ch) {
        _renderGridLinePartialX(0, mode.tw, j2, jch, force);
    }
}

void VgaTerminal::_renderGridLinePartialX(const uint8_t x1, const uint8_t x2, const int yw, const int ych, const bool force)
{
    for (int i = x1, i2 = yw + x1; i < x2; i++, i2++) {
        _terminalChar_t tc;
        {
            std::lock_guard lck(_pGridMutex);
            tc = _pGrid[i2];
        }
        if (!force && tc.rendered) {
            continue;
        }

        SDL_Point p = { i * mode.cw, ych };
        _renderFontChar(p, tc);
    }
}

void VgaTerminal::render(const bool force)
{
    if (!force && (SDL_GetWindowFlags(getWindow()) & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN) {
        return;
    }

    uint8_t curY = _curY;
    uint8_t curX = _curX;
    
    int yw = curY * mode.tw;
    int ych = curY * mode.ch;
    int icur = static_cast<int>(curY) * mode.tw + curX;
    _terminalChar_t tc;
    {
        std::lock_guard lck(_pGridMutex);
        tc = _pGrid[icur];
    }
    SDL_Point p = { curX * mode.cw, ych };
    
    // top cursor grid
    _renderGridPartialY(0, curY, force);
    // left cursor grid
    _renderGridLinePartialX(0, curX, yw, ych,  force);
    if ((force || !tc.rendered)
        && (showCursor && (!_onIdle || _drawCursor))) {
        _renderCursor(p, tc);
    }
    else {
        _renderFontChar(p, tc);
    }
    // right cursor grid
    _renderGridLinePartialX(curX + 1, mode.tw, yw, ych, force);
    // bottom cursor grid    
    _renderGridPartialY(curY + 1, mode.th, force);

    renderPresent();
}

void VgaTerminal::clear() noexcept
{
    const int vy = _viewPortY + _viewPortHeight;
    const int vx = _viewPortX + _viewPortWidth;

    {
        std::lock_guard lck(_pGridMutex);
        for (int j = _viewPortY; j < vy; j++) {
            int j2 = j * mode.tw;
            for (int i = _viewPortX; i < vx; i++) {
                _pGrid[static_cast<size_t>(i) + j2] = _defaultNullChar;
            }
        }
    }

    gotoXY(0, 0);
}

void VgaTerminal::moveCursorLeft() noexcept
{
    if (_curX > _viewPortX) {
        --_curX;
    }
    else if(_curY > _viewPortY) {
        --_curY;
        _curX =_viewPortX + _viewPortWidth - 1;
    }
    else {
        // alredy in 0,0 ... what should i do? :)
    }
    _busy();
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
    _busy();
}

void VgaTerminal::moveCursorDown() noexcept
{
    if (_curY < _viewPortY + _viewPortHeight - 1) {
        ++_curY;
    }
    
    _busy();
}

void VgaTerminal::newLine() noexcept
{
    _curX = _viewPortX;
    uint8_t oldY = _curY;
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
    SDL_Rect r;
    
    r.x = _viewPortX,
    r.y = _viewPortY,
    r.w = _viewPortWidth,
    r.h = _viewPortHeight;

    return r;
}

void VgaTerminal::resetViewport() noexcept
{
    setViewPort(0, 0, mode.tw, mode.th);
}

void VgaTerminal::_busy() noexcept
{
    _onIdle = false;
    // TODO these 4 lines below should be promoted to a method and reused also in the timer routine
    int icur = _curY * mode.tw + _curX;
    {
        std::lock_guard lck(_pGridMutex);
        _pGrid[icur].rendered = false;
    }
}

uint32_t VgaTerminal::_timerCallBackWrapper(uint32_t interval, void* param)
{
    VgaTerminal* that = reinterpret_cast<VgaTerminal*>(param);
    return that->_timerCallBack(interval);
}

uint32_t VgaTerminal::_timerCallBack(uint32_t interval)
{
    // TODO review the user event
    SDL_Event event;
    SDL_UserEvent userevent;
    
    if (_onIdle) {
        _drawCursor = !_drawCursor;
    }
    else {
        _onIdle = true;
        _drawCursor = true;
    }
    // TODO wrap in a cursor function these ops: icur and set rendered flag to false, or in 2
    int icur = _curY * mode.tw + _curX;
    {
        std::lock_guard lck(_pGridMutex);
        _pGrid[icur].rendered = false;
    }

    interval = cursor_time;
        
    userevent.code = 0;
    userevent.data1 = userevent.data2 = NULL;
    event.type = userevent.type = SDL_USEREVENT;
    event.user = userevent;
    userevent.windowID = event.window.windowID = getWindowId();

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
        _curX = _viewPortX;
        _scrollDownGrid();
    }
    else {
        //already at the max
    }

    _busy();
}

void VgaTerminal::_scrollDownGrid() noexcept
{
    // todo can be used as private fields instead of local vars.
    auto vh = _viewPortY + _viewPortHeight;
    auto vw = _viewPortWidth + _viewPortWidth;

    std::lock_guard lck(_pGridMutex);
    for (int j = _viewPortY + 1; j < vh ; j++)
    {
        int j2 = j * mode.tw;
        int jj2 = j2 - mode.tw;
        
        for (int i = _viewPortX; i < vw; i++)
        {
            int i2 = i + j2;
            int ii2 = i + jj2;

            if (_pGrid[i2].rendered && _pGrid[i2] == _pGrid[ii2]) {
                continue;
            }
            
            _pGrid[ii2] = _pGrid[i2];
            _pGrid[ii2].rendered = false;
        }
    }

    // clear line -> TODO: promote to a public method?
    int j2 = (vh - 1) * mode.tw + _viewPortX;
    for (int i = 0; i < _viewPortWidth; i++) {
        _pGrid[static_cast<uint64_t>(i) + j2] = _defaultNullChar;
    }
}

const VgaTerminal::videoMode_t VgaTerminal::getMode() const noexcept
{
    return mode;
}
