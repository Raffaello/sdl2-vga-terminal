#pragma once


#include <stdexcept>

namespace exceptions
{
	class LogicalRenderError : public std::runtime_error
	{
	public:
		LogicalRenderError(const std::string& message) : runtime_error(message) {}
	};

	class SdlWasNotInit : public std::runtime_error
	{
	public:
		SdlWasNotInit(const std::string& message) : runtime_error(message) {}
	};

	class WindowError : public std::runtime_error
	{
	public:
		WindowError(const std::string& message) : runtime_error(message) {}
	};

	class RendererError : public std::runtime_error
	{
	public:
		RendererError(const std::string& message) : runtime_error(message) {}
	};
}
