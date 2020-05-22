#pragma once
#include <string>
#include <utility>

namespace tui
{
    /**
     * @width  is the size of the line.
     * @return the position where it start and end the middle string
     */
    std::pair<uint8_t, uint8_t> align3Strings(const std::string& left, const std::string& middle, const std::string& right, const uint8_t width);
    std::string alignString(const std::string& str, const uint8_t pad, const uint8_t fill) noexcept;
} // namespace tui
