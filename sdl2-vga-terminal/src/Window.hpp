#pragma once

#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <functional>

class Window 
{
public:
	Window() = delete;
	Window(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags);
	
	SDL_Window* getWindow() const;
	SDL_Renderer* getRenderer() const;

	void setRendererDrawColor(const uint8_t r,const uint8_t g, const  uint8_t b, const  uint8_t a) const;
	void renderClear() const;
	void renderPresent() const;
	void toggleFullscreenDesktop();

	uint32_t getWindowId() const;
	
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
