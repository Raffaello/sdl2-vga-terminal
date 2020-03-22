#include <Window.hpp>
#include <stdexcept>

Window::Window(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags)
{
	if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
		throw std::runtime_error("video wasn't inited");
	}

	_pWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, winFlags);

	if (!_pWindow) {
		std::string str = SDL_GetError();
		throw std::runtime_error("Cannot create Window: " + str);
	}

	_pRenderer = SDL_CreateRenderer(_pWindow, drvIndex, renFlags);

	if (!_pRenderer) {
		std::string str = SDL_GetError();
		throw std::runtime_error("Cannot create Renderer: " + str);
	}
	
	setRendererDrawColor(0, 0, 0, 255);
	renderClear();
	renderPresent();
}



inline void Window::setRendererDrawColor(const uint8_t r, const  uint8_t g, const  uint8_t b, const  uint8_t a) const
{
	SDL_SetRenderDrawColor(_pRenderer, r, g, b, a);
}

inline void Window::renderClear() const
{
	SDL_RenderClear(_pRenderer);
}

inline void Window::renderPresent() const
{
	SDL_RenderPresent(_pRenderer);
}

Window::~Window()
{
	if (_pRenderer != nullptr) {
		SDL_DestroyRenderer(_pRenderer);
	}

	if (_pWindow != nullptr) {
		SDL_DestroyWindow(_pWindow);
	}
}
