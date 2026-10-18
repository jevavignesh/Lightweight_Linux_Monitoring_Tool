#include "lmon/version.hpp"

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <string>

namespace {

void print_info(bool as_json) {
    if (as_json) {
        const nlohmann::json info = {
            {"version", lmon::version_string},
            {"build_type", lmon::build_type},
            {"compiler", lmon::compiler},
        };
        std::cout << info.dump(2) << '\n';
        return;
    }

    std::cout << "lmon " << lmon::version_string << '\n'
              << "  build type: " << lmon::build_type << '\n'
              << "  compiler:   " << lmon::compiler << '\n';
}

} // namespace

int main(int argc, char** argv) {
    CLI::App app{"lmon - lightweight Linux monitoring tool", "lmon"};
    app.set_version_flag("-V,--version", std::string{lmon::version_string});
    app.require_subcommand(0, 1);

    std::string log_level = "info";
    app.add_option("--log-level", log_level, "Logging verbosity")
        ->check(CLI::IsMember({"trace", "debug", "info", "warn", "error", "off"}))
        ->capture_default_str();

    bool info_json = false;
    auto* info = app.add_subcommand("info", "Show version and build information");
    info->add_flag("--json", info_json, "Print as JSON");

    CLI11_PARSE(app, argc, argv);

    spdlog::set_level(spdlog::level::from_str(log_level));
    spdlog::debug("lmon {} starting", lmon::version_string);

    if (info->parsed()) {
        print_info(info_json);
        return 0;
    }

    std::cout << app.help();
    return 0;
}
