#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace lmon {

// Read-only access to /proc and /sys under a configurable root.
//
// Production code uses the default root "/". Tests pass a fixtures directory,
// so collectors can be tested against known file contents on any machine.
class ProcFs {
public:
    explicit ProcFs(std::filesystem::path root = "/");

    [[nodiscard]] const std::filesystem::path& root() const noexcept { return root_; }

    // Maps a path such as "/proc/stat" or "proc/stat" to <root>/proc/stat.
    [[nodiscard]] std::filesystem::path resolve(std::string_view relative) const;

    // Whole file contents, or nullopt if the file can't be opened or read.
    [[nodiscard]] std::optional<std::string> read_file(std::string_view relative) const;

    // File contents split into lines (without the trailing '\n').
    [[nodiscard]] std::optional<std::vector<std::string>> read_lines(
        std::string_view relative) const;

private:
    std::filesystem::path root_;
};

} // namespace lmon
