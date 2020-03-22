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
	
	virtual ~Window();
private:
	SDL_Window* _pWindow;
	SDL_Renderer* _pRenderer;
};