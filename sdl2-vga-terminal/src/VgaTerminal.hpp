#pragma once

#if defined(_MSC_VER) && (_MSC_VER < 1910 || _MSC_FULL_VER < 190023918)
#   error "Visual Studio 2015 Update 2 at least is required"
#endif

#include "Window.hpp"
#include <memory>
#include <string>
#include <bitset>
#include <atomic>


class VgaTerminal : public Window
{
public:
    typedef struct
    {
        uint8_t  mode;      // video mode (only mode 3 available at the moment)
        uint8_t  tw;        // terminal width
        uint8_t  th;        //          height
        uint8_t  cw;        // char     width
        uint8_t  ch;        //          height  | font size
        uint16_t numColors; // 65K max (16 bit) otherwse 24|32 bits if 0 (?)
        uint8_t* font;
        uint8_t* palette;   // RGB palette
        uint8_t* cursors;
    } videoMode_t;

    typedef struct
    {
        uint8_t c;
        uint8_t col;
        uint8_t bgCol;
    } terminalChar_t;

    typedef std::pair<uint8_t, uint8_t> position_t;

    enum class CURSOR_MODE : uint8_t {
        CURSOR_MODE_NORMAL = 0,
        CURSOR_MODE_FAT = 1,
        CURSOR_MODE_BLOCK = 2,
        CURSOR_MODE_VERTICAL = 3,
    };

    CURSOR_MODE cursor_mode = CURSOR_MODE::CURSOR_MODE_NORMAL;

    static const std::string getVersion();

    VgaTerminal() = delete;
    VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags);
    VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    virtual ~VgaTerminal();

    void gotoXY(const uint8_t x, const uint8_t y) noexcept;
    void gotoXY(const position_t &position) noexcept;
  
    position_t getXY() const noexcept;
    uint8_t getX() const noexcept;
    uint8_t getY() const noexcept;
    
    void write(const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept;
    void write(const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept;
    void writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept;
    terminalChar_t at(const uint8_t x, const uint8_t y) const noexcept;

    void render(const bool force = false);
    void clear() noexcept;
  
    void moveCursorLeft() noexcept;
    void moveCursorRight() noexcept;
    void moveCursorUp() noexcept;
    void moveCursorDown() noexcept;

    void newLine() noexcept;

    bool setViewPort(const position_t& viewport, const uint8_t width, const uint8_t height) noexcept;
    bool setViewPort(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height) noexcept;
    bool setViewPort(const SDL_Rect& r) noexcept;
    SDL_Rect getViewport() const noexcept;
    void resetViewport() noexcept;

    const videoMode_t getMode() const noexcept;
    
    uint8_t cursorDefaultCol = 7;
    uint16_t cursor_time = 500; /// ms
    bool showCursor = true;
    bool autoScroll = true;
protected:

private:
    typedef struct _terminalChar_t : terminalChar_t
    {
        bool rendered;
        bool operator==(const _terminalChar_t& o) const;
    } _terminalChar_t;

    static const videoMode_t mode3;
    std::unique_ptr<SDL_Color[]> pCol;
    SDL_Palette _pal;
    videoMode_t mode;
    std::unique_ptr<std::atomic<_terminalChar_t>[]> _pGrid;
    //std::unique_ptr<_terminalChar_t[]> _pGrid;
    const _terminalChar_t _defaultNullChar = _terminalChar_t({ 0, 0, 0, false });
    
    std::atomic<uint8_t> _curX = 0;
    std::atomic<uint8_t> _curY = 0;
    uint8_t _viewPortX;
    uint8_t _viewPortWidth;
    uint8_t _viewPortY;
    uint8_t _viewPortHeight;
  
    std::atomic<bool> _drawCursor = true; 
    SDL_TimerID _cursorTimerId = 0;
    //std::mutex _cursortTimerMutex;
    std::atomic<bool> _onIdle = true;
    // TODO: find a better name for the method.
    void _busy() noexcept;
    static uint32_t _timerCallBackWrapper(uint32_t interval, void* param);
    uint32_t _timerCallBack(uint32_t interval);

    void _incrementCursorPosition(bool increment = true) noexcept;
    void _scrollDownGrid() noexcept;
    void _renderFontChar(const SDL_Point& dst, _terminalChar_t& tc);
    void _renderCharLine(const std::bitset<8>& line, const int dstx, const int dsty, const uint8_t col, const uint8_t bgCol);
    void _renderCursor(const SDL_Point&dst, _terminalChar_t& tc);
    void _renderGridPartialY(const uint8_t y1, const uint8_t y2, const bool force);
    void _renderGridLinePartialX(const uint8_t x1, const uint8_t x2, const int yw, const int ych, const bool force);
};
