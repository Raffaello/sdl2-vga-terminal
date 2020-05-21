#include "utils.hpp"
#include <stdexcept>

namespace tui
{
std::pair<uint8_t, uint8_t> align3Strings(const std::string& left, const std::string& middle, const std::string& right, const uint8_t width)
{
    constexpr uint8_t markers = 2;
    constexpr uint8_t minWidth = 3 + markers;
    const uint8_t ls = static_cast<uint8_t>(left.size());
    const uint8_t ms = static_cast<uint8_t>(middle.size());
    const uint8_t rs = static_cast<uint8_t>(right.size());
    const uint8_t stringsSize = ls + ms + rs;

    if (ls == 0 || ms == 0 || rs == 0) {
        throw std::invalid_argument("no strings can be empty");
    }

    if (width < minWidth) {
        throw std::invalid_argument("width has to be at least " + std::to_string(minWidth));
    }

    if (stringsSize > width - markers) {
        // TODO balancing string adding "..." to their max size. ?
        throw std::invalid_argument("string truncation not yet implemented");
    }

    const uint8_t wss = width - stringsSize;
    const uint8_t s3 = static_cast<uint8_t>(wss / 3);
    const uint8_t s3r = ((width - stringsSize) % 3) != 0;

    const uint8_t lpad = s3;
    const uint8_t mpad = s3 + s3r;

    const uint8_t first = lpad + ls;
    const uint8_t second = first + ms + mpad;

    return std::make_pair(first, second);
}

std::string alignString(const std::string& str, const uint8_t pad, const uint8_t fill) noexcept
{
    uint8_t p2 = pad / 2;
    uint8_t pr = pad % 2;

    return std::string(p2, fill) + str + std::string(static_cast<uint8_t>(p2 + pr), fill);
}
} // namespace tui
