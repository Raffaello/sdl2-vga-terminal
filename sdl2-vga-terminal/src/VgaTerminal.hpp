#pragma once

#include "Window.hpp"
#include <memory>
#include <string>
#include <bitset>


class VgaTerminal : public Window
{

public:
    typedef struct
    {
        uint8_t  mode;    // video mode (only mode 3 available at the moment)
        uint8_t  tw;      // terminal width
        uint8_t  th;      //          hieght
        uint8_t  cw;      // char     width
        uint8_t  ch;      //          height  | font size
        uint8_t* font;
        int  numColors;
        uint8_t* palette; // RGB palette assumed (might be required a palette format flag?)
    } videoMode_t;

    // TODO keep only the 3 uint8_t here,
    //      create a private one that embed this one and the other 2 bools.
    typedef struct terminalChar_t
    {
        uint8_t c;
        uint8_t col;
        uint8_t bgCol;
        bool rendered;
        bool operator==(const terminalChar_t& o) const;
    } terminalChar_t;

    typedef std::pair<uint8_t, uint8_t> position_t;

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
    /// the X,Y are relative to the new viewport.
    /// TODO: return a bool if the viewport is succesfully set or not.
    void setViewPort(const position_t& viewport, const uint8_t width, const uint8_t height) noexcept;
    void setViewPort(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height) noexcept;
    void setViewPort(const SDL_Rect& r) noexcept;
    SDL_Rect getViewport() const noexcept;
    
    uint8_t curDefaultCol = 7;
    bool showCursor = true;
    bool autoScroll = true;
    
    const videoMode_t getMode() const noexcept;
private:
    static const videoMode_t mode3;
    std::unique_ptr<SDL_Color[]> pCol;
    SDL_Palette p;
    videoMode_t mode;
    uint8_t _curX = 0;
    uint8_t _curY = 0;
    std::unique_ptr<terminalChar_t[]> _pGrid;
    terminalChar_t defaultNullChar = { 0, 0, 0, false };
    uint8_t _viewPortX;
    uint8_t _viewPortWidth;
    uint8_t _viewPortY;
    uint8_t _viewPortHeight;

    bool _cursonOn = true;
    SDL_TimerID _cursorTimer = 0;
    
    // these should be parameters?
    uint8_t cur_shape = 219;
    uint32_t cursor_time = 1000;
 
    uint32_t _timerId = 0;
    static uint32_t _timerCallBack(uint32_t interval, void* param);

    void incrementCursorPosition() noexcept;
    void scrollDownGrid() noexcept;
    void renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c);
};
