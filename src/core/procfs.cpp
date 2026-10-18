#include "lmon/procfs.hpp"

#include <fstream>
#include <iterator>
#include <utility>

namespace lmon {

ProcFs::ProcFs(std::filesystem::path root) : root_(std::move(root)) {}

std::filesystem::path ProcFs::resolve(std::string_view relative) const {
    while (!relative.empty() && relative.front() == '/') {
        relative.remove_prefix(1);
    }
    return root_ / relative;
}

std::optional<std::string> ProcFs::read_file(std::string_view relative) const {
    std::ifstream in(resolve(relative), std::ios::binary);
    if (!in.is_open()) {
        return std::nullopt;
    }

    // Files in /proc report a size of 0, so read until EOF instead of
    // asking for the file size up front.
    std::string contents{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    if (in.bad()) {
        return std::nullopt;
    }
    return contents;
}

std::optional<std::vector<std::string>> ProcFs::read_lines(std::string_view relative) const {
    auto contents = read_file(relative);
    if (!contents) {
        return std::nullopt;
    }

    std::vector<std::string> lines;
    std::string_view rest = *contents;
    while (!rest.empty()) {
        const auto newline = rest.find('\n');
        lines.emplace_back(rest.substr(0, newline));
        if (newline == std::string_view::npos) {
            break;
        }
        rest.remove_prefix(newline + 1);
    }
    return lines;
}

} // namespace lmon
