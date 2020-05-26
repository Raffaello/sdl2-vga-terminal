#pragma once


#include "Window.hpp"
#include <memory>
#include <string>
#include <bitset>
#include <atomic>
#include <mutex>
#include <utility>


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

    // TODO major version refactor to a struct
    // BODY instead of having first and second
    // BODY have a struct with x, y.
    typedef std::pair<uint8_t, uint8_t> position_t;

    enum class CURSOR_MODE : uint8_t {
        CURSOR_MODE_NORMAL = 0,
        CURSOR_MODE_FAT = 1,
        CURSOR_MODE_BLOCK = 2,
        CURSOR_MODE_VERTICAL = 3,
    };
    static constexpr uint8_t NUM_CURSOR_MODES = 4;
    CURSOR_MODE cursor_mode = CURSOR_MODE::CURSOR_MODE_NORMAL;

    enum class CURSOR_SPEED : uint16_t {
        CURSOR_SPEED_NORMAL = 500,
        CURSOR_SPEED_FAST = 250,
        CURSOR_SPEED_SLOW = 750
    };
    static constexpr uint8_t NUM_CURSOR_SPEEDS = 3;

    static const std::string getVersion();

    VgaTerminal() = delete;
    VgaTerminal(const VgaTerminal&) = delete;
    VgaTerminal(const VgaTerminal&&) = delete;
    VgaTerminal& operator=(const VgaTerminal&) = delete;
    VgaTerminal& operator=(const VgaTerminal&&) = delete;
    explicit VgaTerminal(const std::string& title);
    VgaTerminal(const std::string& title, const int winFlags, const int drvIndex, const int renFlags);
    VgaTerminal(const std::string& title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    VgaTerminal(const std::string& title, const int x, const int y, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    virtual ~VgaTerminal();

    void gotoXY(const uint8_t x, const uint8_t y) noexcept;
    void gotoXY(const position_t &position) noexcept;

    position_t getXY() const noexcept;
    uint8_t getX() const noexcept;
    uint8_t getY() const noexcept;

    void write(const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept;
    void write(const terminalChar_t& tc) noexcept;
    void write(const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept;
    
    void writeXY(const uint8_t x, const uint8_t y, const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept;
    void writeXY(const position_t& pos, const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept;
    void writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept;
    void writeXY(const position_t& pos, const std::string &str, const uint8_t col, const uint8_t bgCol) noexcept;
    void writeXY(const uint8_t x, const uint8_t y, const terminalChar_t& tc) noexcept;
    void writeXY(const position_t& pos, const terminalChar_t& tc) noexcept;

    terminalChar_t at(const uint8_t x, const uint8_t y) noexcept;

    void render(const bool force = false);
    void clear() noexcept;
    void clearLine(const uint8_t y, const uint8_t bgCol = 0) noexcept;
    void fill(const uint8_t c, const uint8_t col, const uint8_t bgCol) noexcept;

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
    bool isIdle() const noexcept;

    // NOTE: if you are using the methods do not use cursor_time field.
    void setCursorSpeed(const CURSOR_SPEED speed) noexcept;
    CURSOR_SPEED getCursorSpeed() const noexcept;
    // TODO not really sure what is this for anymore...
    uint8_t cursorDefaultCol = 7;
    // @deprecated to be removed in 1
    std::atomic<uint16_t> cursor_time = static_cast<uint16_t>(CURSOR_SPEED::CURSOR_SPEED_NORMAL); /// ms
    bool showCursor = true;
    bool blinkCursor = true;
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
    // potentially candidate for atomic, when setMode would be available
    // at the moment is like a const, so defined as a const...
    const videoMode_t mode;
    std::unique_ptr<_terminalChar_t[]> _pGrid;
    std::mutex _pGridMutex;
    const _terminalChar_t _defaultNullChar = _terminalChar_t({ 0, 0, 0, false });
    std::mutex _renderMutex;

    std::atomic<uint8_t> _curX = 0;
    std::atomic<uint8_t> _curY = 0;
    std::atomic<uint8_t> _viewPortX;
    std::atomic<uint8_t> _viewPortWidth;
    std::atomic<uint8_t> _viewPortY;
    std::atomic<uint8_t> _viewPortHeight;
    std::atomic<uint8_t> _viewPortX2;     /// _viewportX + _viewportWidth derived value
    std::atomic<uint8_t> _viewPortY2;     /// _viewportY + _viewportHeight derived value

    CURSOR_SPEED _cursor_speed = CURSOR_SPEED::CURSOR_SPEED_NORMAL;
    //std::atomic<uint16_t> _cursor_time = static_cast<uint16_t>(_cursor_speed); /// ms
    std::atomic<bool> _drawCursor = true;
    SDL_TimerID _cursorTimerId = 0;
    std::atomic<bool> _onIdle = true;
    void _setBusy() noexcept;
    static uint32_t _timerCallBackWrapper(uint32_t interval, void* param);
    uint32_t _timerCallBack(uint32_t interval);

    void _incrementCursorPosition(const bool increment = true) noexcept;
    void _scrollDownGrid() noexcept;
    void _renderFontChar(const SDL_Point& dst, _terminalChar_t& tc);
    void _renderCharLine(const std::bitset<8>& line, const int dstx, const int dsty, const uint8_t col, const uint8_t bgCol);
    void _renderCursor(const SDL_Point&dst, const _terminalChar_t& tc);
    void _renderGridPartialY(const uint8_t y1, const uint8_t y2, const bool force);
    void _renderGridLinePartialX(const uint8_t x1, const uint8_t x2, const int yw, const int ych, const bool force);
    _terminalChar_t _getCharAt(const size_t pos) noexcept;
    const int _getCursorPosition() const noexcept;
    _terminalChar_t _getCursorChar() noexcept;
    void _setCharAtCursorPosition(const _terminalChar_t& tc) noexcept;
    void _setCursorNotRendered() noexcept;
};
