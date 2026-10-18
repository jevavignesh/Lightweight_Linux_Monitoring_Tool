#pragma once

#include <nlohmann/json.hpp>

#include <string_view>

namespace lmon {

// A source of metrics (CPU, memory, disk, ...).
//
// The agent and CLI hold collectors as std::unique_ptr<ICollector> and call
// collect() once per interval. Collectors that report rates (CPU %, bytes/s)
// keep the previous raw counters between calls and return deltas.
class ICollector {
public:
    ICollector() = default;
    virtual ~ICollector() = default;

    ICollector(const ICollector&) = delete;
    ICollector& operator=(const ICollector&) = delete;
    ICollector(ICollector&&) = delete;
    ICollector& operator=(ICollector&&) = delete;

    // Short, stable identifier used as the JSON key, e.g. "cpu".
    [[nodiscard]] virtual std::string_view name() const = 0;

    // Takes one sample.
    [[nodiscard]] virtual nlohmann::json collect() = 0;
};

} // namespace lmon
