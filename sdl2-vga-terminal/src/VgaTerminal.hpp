#pragma once

#include <Window.hpp>
#include <memory>
#include <string>
#include <bitset>

class VgaTerminal : public Window
{

public:
    typedef struct videoMode_t
    {
        uint8_t  mode; // video mode (only mode 3 available at the moment)
        //uint16_t sw;   // screen   width
        //uint16_t sh;   //          height
        uint8_t  tw;   // terminal width
        uint8_t  th;   //          hieght
        uint8_t  cw;   // char     width
        uint8_t  ch;   //          height  | font size
        //uint8_t  fs;   // font size 
        uint8_t* font;
        int  numColors;
        uint8_t* palette; // BGR palette assumed (might be required a palette format flag?)
    } videoMode_t;

    typedef struct terminalChar_t
    {
        uint8_t c;
        uint8_t col;
        uint8_t bgCol;
        bool rendered;
        bool operator==(const terminalChar_t& o) const;
    } terminalChar_t;

    typedef std::pair<uint8_t, uint8_t> position_t;

    static const videoMode_t mode3;

    VgaTerminal() = delete;
    // declared otherwise not compiling... not implemented
    VgaTerminal(const VgaTerminal& t);
    VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags);
    VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    virtual ~VgaTerminal();

    void gotoXY(const uint8_t x, const uint8_t y);
    void gotoXY(const position_t &position);
    
    position_t getXY() const;
    uint8_t getX() const;
    uint8_t getY() const;
    
    void write(const char c, const uint8_t col, const uint8_t bgCol);
    void write(const std::string &str, const uint8_t col, const uint8_t bgCol);
    void writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol);
    terminalChar_t at(const uint8_t x, const uint8_t y) const;

    void render(const bool force = false);
    void clearGrid();
    void moveCursorLeft() noexcept;
    void moveCursorRight() noexcept;
    void moveCursorUp() noexcept;
    void moveCursorDown() noexcept;
    
    uint8_t curDefaultCol = 7;
    bool showCursor = true;

private:
    std::unique_ptr<SDL_Color[]> pCol;
    SDL_Palette p;
    videoMode_t mode;
    uint8_t _curX = 0;
    uint8_t _curY = 0;
    std::unique_ptr<terminalChar_t[]> _pGrid;
    terminalChar_t defaultNullChar = { 0, 0, 0, false };
   
    bool _cursonOn = true;
    SDL_TimerID _cursorTimer = 0;
    
    // these should beparameters?
    uint8_t cur_shape = 219;
    uint32_t cursor_time = 1000;

    static uint32_t _timerCallBack(uint32_t interval, void* param);

    void incrementCursorPosition();
    void scrollDownGrid();
    void renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c);
};
