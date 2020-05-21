#pragma once

#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <functional>

class Window
{
public:
    Window() = delete;
    Window(const Window&) = delete;
    Window(const Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(const Window&&) = delete;
    Window(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    Window(const std::string& title, const int x, const int y, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
    SDL_Window* getWindow() const noexcept;
    SDL_Renderer* getRenderer() const noexcept;

    void setRendererDrawColor(const uint8_t r,const uint8_t g, const  uint8_t b, const  uint8_t a) const noexcept;
    void renderDrawPoints(const SDL_Color& color, const SDL_Point points[], int count) const noexcept;

    void renderClear() const noexcept;
    void renderPresent() const noexcept;
    void toggleFullscreenDesktop() noexcept;

    uint32_t getWindowId() const noexcept;

    typedef std::function<bool(SDL_Event&, Window*)> handler_t;
    handler_t handler = nullptr;
    bool handleEvent(SDL_Event& event);

    virtual ~Window();
private:
    SDL_Window* _pWindow = nullptr;
    SDL_Renderer* _pRenderer = nullptr;
    uint32_t _windowId;
    bool _bFullScreenDesktop = false;
};
