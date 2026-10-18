#include "lmon/strings.hpp"

namespace lmon {

namespace {

constexpr std::string_view whitespace = " \t\n\r\f\v";

} // namespace

std::string_view trim(std::string_view text) noexcept {
    const auto first = text.find_first_not_of(whitespace);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

std::vector<std::string_view> split_whitespace(std::string_view text) {
    std::vector<std::string_view> fields;
    std::size_t pos = 0;
    while (true) {
        const auto start = text.find_first_not_of(whitespace, pos);
        if (start == std::string_view::npos) {
            break;
        }
        const auto end = text.find_first_of(whitespace, start);
        if (end == std::string_view::npos) {
            fields.push_back(text.substr(start));
            break;
        }
        fields.push_back(text.substr(start, end - start));
        pos = end;
    }
    return fields;
}

} // namespace lmon
