#pragma once

#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>

namespace lmon {

// Removes leading and trailing whitespace.
[[nodiscard]] std::string_view trim(std::string_view text) noexcept;

// Splits on runs of whitespace, skipping empty fields. The returned views
// point into `text`, so `text` must outlive them.
[[nodiscard]] std::vector<std::string_view> split_whitespace(std::string_view text);

// Parses the whole of `text` (after trimming) as a number.
// Returns nullopt on empty input, trailing garbage or overflow.
template <typename T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]] std::optional<T> parse_number(std::string_view text) noexcept {
    text = trim(text);
    if (text.empty()) {
        return std::nullopt;
    }

    T value{};
    const char* const end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);
    if (ec != std::errc{} || ptr != end) {
        return std::nullopt;
    }
    return value;
}

} // namespace lmon
