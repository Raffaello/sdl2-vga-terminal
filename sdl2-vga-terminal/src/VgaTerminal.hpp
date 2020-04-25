#pragma once

#include "Window.hpp"
#include <memory>
#include <string>


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
    } videoMode_t;

    typedef struct
    {
        uint8_t c;
        uint8_t col;
        uint8_t bgCol;
    } terminalChar_t;

    typedef std::pair<uint8_t, uint8_t> position_t;

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
    
    void write(const char c, const uint8_t col, const uint8_t bgCol) noexcept;
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
    
    uint8_t curDefaultCol = 7;
    bool showCursor = true;
    bool autoScroll = true;
    
    const videoMode_t getMode() const noexcept;
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
    std::unique_ptr<_terminalChar_t[]> _pGrid;
    _terminalChar_t defaultNullChar = { 0, 0, 0, false };
    
    uint8_t _curX = 0;
    uint8_t _curY = 0;
    uint8_t _viewPortX;
    uint8_t _viewPortWidth;
    uint8_t _viewPortY;
    uint8_t _viewPortHeight;
  
    // these should be parameters?
    uint8_t cur_shape = 219;
    uint32_t cursor_time = 500; // uint16_t?
    uint8_t _cursor_mode = 0;
    
    bool _cursonOn = true;
    SDL_TimerID _timerId = 0;
    static uint32_t _timerCallBack(uint32_t interval, void* param);

    void incrementCursorPosition(bool increment = true) noexcept;
    void scrollDownGrid() noexcept;
    void renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c);
};
