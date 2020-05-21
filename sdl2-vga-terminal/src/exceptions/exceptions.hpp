#pragma once


#include <stdexcept>
#include <string>

namespace exceptions
{
    class LogicalRenderError : public std::runtime_error
    {
    public:
        explicit LogicalRenderError(const std::string& message) : runtime_error(message) {}
    };

    class SdlWasNotInit : public std::runtime_error
    {
    public:
        explicit SdlWasNotInit(const std::string& message) : runtime_error(message) {}
    };

    class WindowError : public std::runtime_error
    {
    public:
        explicit WindowError(const std::string& message) : runtime_error(message) {}
    };

    class RendererError : public std::runtime_error
    {
    public:
        explicit RendererError(const std::string& message) : runtime_error(message) {}
    };
} // namespace exceptions
