#pragma once

#include <Window.hpp>
#include <memory>
#include <string>

class VgaTerminal : public Window
{

public:
    typedef struct videoMode_t
    {
        uint8_t  mode;
        uint16_t sw;
        uint16_t sh;
        uint8_t  tw;
        uint8_t  th;
        uint8_t  cw;
        uint8_t  ch;
    } VideoMode;

    typedef struct terminalChar_t
    {
        uint8_t c;
        uint8_t col;
        uint8_t bgCol;
        bool rendered;
    } terminalChar_t;

    typedef std::pair<uint8_t, uint8_t> position_t;

    static const videoMode_t mode3;

    VgaTerminal() = delete;
    VgaTerminal(const std::string &title, const int winFlags, const int drvIndex, const int renFlags);
    VgaTerminal(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    
    void gotoXY(const uint8_t x, const uint8_t y);
    void gotoXY(const position_t &position);
    
    position_t getXY() const;
    uint8_t getX() const;
    uint8_t getY() const;
    
    void write(const char c, const uint8_t col, const uint8_t bgCol);
    void write(const std::string &str, const uint8_t col, const uint8_t bgCol);
    void writeXY(const uint8_t x, const uint8_t y, const std::string &str, const uint8_t col, const uint8_t bgCol);
   /*
   return defaultNullChar if outside the terminal
   */
    terminalChar_t at(const uint8_t x, const uint8_t y) const;

    void render();
    void clearGrid();

private:
    std::unique_ptr<SDL_Color[]> pCol;
    SDL_Palette p;
    videoMode_t mode;
    uint8_t _curX = 0;
    uint8_t _curY = 0;
    std::unique_ptr<terminalChar_t[]> _pGrid;
    terminalChar_t defaultNullChar = { 0, 0, 0, false };

    void incrementCursorPosition();
    void scrollDownGrid();
    void renderChar(const SDL_Point& dst, const uint8_t col, const uint8_t bgCol, const char c);
};
