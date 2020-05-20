/*************************************************************
 ***           Text User Interface utilities               ***
 *** ===================================================== ***
 *** Some basic TUI components                             ***
 *** if growing, to be promoted as a stand-alone library   ***
 *** or could be just a component of this library          ***
 *** ----------------------------------------------------- ***
 *** Description:                                          ***
 *** using VgaTerminal in mode3                            ***
 *** - no other method allowed                             ***
 *** - no access to VgaTerminal                            ***
 *** - no access to Window                                 ***
 *** - working with default palette colors only            ***
 *** - Header display a title, description and TUI Version ***
 *** - background                                          *** 
 *** - fix Footer                                          ***
 *** - no access to Window or VgaTerminal                  ***
 *** ===================================================== ***
 ***                  VERSION 0.1-showcase                 ***
 *************************************************************/
#pragma once

#include "VgaTerminal.hpp"

class TuiTerminal
{
public:
    static constexpr char* VERSION = "0.1";

    TuiTerminal(const std::string& title, const std::string& description);
    // TODO wrappers to getWindow and renderer
    // BODY I don't like this desing... refactor with an interface
    SDL_Window* getWindow();
    SDL_Renderer* getRenderer();
    void draw();
    void render();
    VgaTerminal::terminalChar_t background_tc = { 177, 8, 9 };
    VgaTerminal::terminalChar_t footer_tc = { 219, 15, 1};
    static constexpr uint8_t defaultCol = 15;
    static constexpr uint8_t defaultBgCol = 1;
    // TODO use a label object (component interface) for the body
    void drawSingleBorderDialog(const SDL_Rect& r, uint8_t col, uint8_t bgCol, const std::string& title, const std::string& body);
    void drawDialog(const SDL_Rect& r, uint8_t col, uint8_t bgCol, const std::string& header, const std::string& body);
    // progress bar, todo as a component
    void progressBar(const bool colored, const bool showProgress, const uint8_t x, const uint8_t y, const uint8_t width, const size_t progress, const size_t max);
protected:
private:
    VgaTerminal _term;
    const std::string _title;
    const std::string _desc;
    
    void _drawBackGround() noexcept;
    void _drawHeader();
    // helpers functions
    void _drawBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str, const uint8_t tc, const uint8_t mc, const uint8_t bc);
    void _drawLeftBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str);
    void _drawMiddleBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str);
    void _drawRightBorder(const SDL_Rect& r, const uint8_t col, const uint8_t bgCol, const std::string& str);
    void _drawHBorder(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t col, const uint8_t bgCol, const uint8_t c) noexcept;
    // -----
    // Shadow methods (split?)
    void _unitShadowColor(uint8_t& col);
    void _shadowColor(VgaTerminal::terminalChar_t& tc);
    void _drawShadow(const SDL_Rect& r) noexcept;
    // footer
    void _drawFooter() noexcept;
    // dialog
    
    
};
