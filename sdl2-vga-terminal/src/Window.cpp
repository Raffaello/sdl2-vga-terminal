#include "Window.hpp"
#include <stdexcept>

Window::Window(const std::string &title, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags)
	: Window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, winFlags, drvIndex, renFlags)
{
}

Window::Window(const std::string& title, const int x, const int y, const int width, const int height, const int winFlags, const int drvIndex, const int renFlags)
{
	if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
		throw std::runtime_error(std::string("video wasn't inited. Error: ") + SDL_GetError());
	}

	if ((winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP) {
		_bFullScreenDesktop = true;
	}
	else {
		_bFullScreenDesktop = false;
	}

	_pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, winFlags);

	if (!_pWindow) {
		std::string str = SDL_GetError();
		throw std::runtime_error("Cannot create Window: " + str);
	}

	_pRenderer = SDL_CreateRenderer(_pWindow, drvIndex, renFlags);

	if (!_pRenderer) {
		std::string str = SDL_GetError();
		throw std::runtime_error("Cannot create Renderer: " + str);
	}

	_windowId = SDL_GetWindowID(getWindow());

	setRendererDrawColor(0, 0, 0, 255);
	renderClear();
	renderPresent();
}

SDL_Window* Window::getWindow() const noexcept
{
	return _pWindow;
}

SDL_Renderer* Window::getRenderer() const noexcept
{
	return _pRenderer;
}

void Window::setRendererDrawColor(const uint8_t r, const  uint8_t g, const  uint8_t b, const  uint8_t a) const noexcept
{
	SDL_SetRenderDrawColor(_pRenderer, r, g, b, a);
}

void Window::renderDrawPoints(const SDL_Color& color, const SDL_Point points[], int count) const noexcept
{
	SDL_SetRenderDrawColor(getRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoints(getRenderer(), points, count);
}

void Window::renderClear() const noexcept
{
	SDL_RenderClear(_pRenderer);
}

void Window::renderPresent() const noexcept
{
	SDL_RenderPresent(_pRenderer);
}

void Window::toggleFullscreenDesktop() noexcept
{
	_bFullScreenDesktop = !_bFullScreenDesktop;
	SDL_SetWindowFullscreen(getWindow(), _bFullScreenDesktop ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
}

uint32_t Window::getWindowId() const noexcept
{
	return _windowId;
}

bool Window::handleEvent(SDL_Event& event)
{
	if (event.window.windowID != getWindowId() || handler == nullptr) {
		return false;
	}

	
	return std::invoke(handler, event, this);
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
