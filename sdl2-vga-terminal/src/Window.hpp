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
	inline void toggleFullscreenDesktop() { 
		_bFullScreenDesktop = !_bFullScreenDesktop;
		SDL_SetWindowFullscreen(getWindow(), _bFullScreenDesktop ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
	}

	inline uint32_t getWindowId() const { return _windowId; }

	virtual ~Window();
private:
	SDL_Window* _pWindow = nullptr;
	SDL_Renderer* _pRenderer = nullptr;
	bool _bFullScreenDesktop = false;
	uint32_t _windowId;
};