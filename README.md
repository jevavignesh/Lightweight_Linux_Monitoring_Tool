# Linux Monitoring Tool (lmon)

A lightweight Linux monitoring tool written in **modern C++ (C++20)** and built with **CMake**. It collects system metrics (CPU, memory, disk, network, processes), shows them live, keeps their history and sends alerts when something crosses a limit you set.

> **Status:** early development. **Phase 0 (build skeleton) is done**: the CMake project, presets, dependencies, `lmon_core` library, `lmon` CLI stub, tests and CI are in place. Everything else below is planned; see the [Roadmap](#roadmap).

---

## Features (planned)

- **Live metrics:** CPU, memory, swap, disk, network and process usage, read directly from `/proc` and `/sys`.
- **Terminal view:** an `htop`-style live dashboard (`lmon top`), plus JSON output for scripts (`lmon snapshot --json`).
- **History:** stores samples in SQLite so you can look back over hours or days.
- **Alerts:** threshold rules (for example, CPU above 90% for 5 minutes) that notify you through webhooks (Slack/Discord/Telegram) or email.
- **Multiple hosts:** a small agent daemon on each machine sends its metrics to a central server.
- **Web dashboard:** a static web page served by the server, with live charts per host.
- **Low overhead:** a single native binary per component with no runtime dependencies on interpreters.

## How it works

```
┌──────────────────┐        HTTP/JSON         ┌──────────────────────┐
│  lmon-agent      │  ─────────────────────▶  │   lmon-server        │
│  (each host)     │   push every N seconds   │  - REST API          │
│  - collectors    │                          │  - SQLite storage    │
│  - local buffer  │                          │  - alert engine      │
└──────────────────┘                          │  - serves web UI     │
                                              └─────────┬────────────┘
                                                        │
                                   ┌────────────────────┼──────────────┐
                                   ▼                    ▼              ▼
                              Web dashboard        lmon (CLI/TUI)  Notifications
```

All three programs are built on a shared static library, **`lmon_core`**, which contains the collectors, data model and config loading.

| Build target | Type | Job |
|---|---|---|
| `lmon_core` | static library | Collectors, metric types, JSON serialization, config. Used by every other target. |
| `lmon` | executable | CLI and terminal UI. Works on its own on a single machine. |
| `lmon-agent` | executable (daemon) | Runs on each host, collects metrics on a timer and pushes them to the server. |
| `lmon-server` | executable (daemon) | Receives and stores metrics, evaluates alert rules, serves the REST API and web dashboard. |
| `lmon_tests` | executable | Unit and integration tests, run through CTest. |

## Metrics collected

| Category | Metrics | Source |
|---|---|---|
| CPU | usage % (total and per core), load average, iowait, steal | `/proc/stat`, `/proc/loadavg` |
| Memory | used, free, cached, buffers, swap | `/proc/meminfo` |
| Disk | usage % per mount, inode usage, read/write IOPS and throughput | `statvfs()`, `/proc/mounts`, `/proc/diskstats` |
| Network | bytes and packets in/out per interface, errors, drops | `/proc/net/dev` |
| Processes | top processes by CPU and memory, total and zombie counts | `/proc/[pid]/stat`, `/proc/[pid]/status` |
| System | hostname, uptime, kernel version, temperatures | `uname()`, `/proc/uptime`, `/sys/class/thermal` |
| Services | systemd unit status (e.g. nginx, sshd) | `systemctl` / sd-bus (later phase) |

## Tech stack

| Concern | Choice |
|---|---|
| Language | C++20 (GCC 11+ or Clang 14+) |
| Build system | CMake 3.25+ with `CMakePresets.json` (Make by default, Ninja optional) |
| Dependency management | CMake `FetchContent` (vcpkg can be added later) |
| Logging | [spdlog](https://github.com/gabime/spdlog) + [fmt](https://github.com/fmtlib/fmt) |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) |
| Config | [yaml-cpp](https://github.com/jbeder/yaml-cpp) |
| Command-line parsing | [CLI11](https://github.com/CLIUtils/CLI11) |
| Terminal UI | [FTXUI](https://github.com/ArthurSonzogni/FTXUI) |
| HTTP client and server | [cpp-httplib](https://github.com/yhirose/cpp-httplib) (OpenSSL for TLS) |
| Storage | SQLite3 via [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp) |
| Testing | [GoogleTest](https://github.com/google/googletest) + CTest |
| Code quality | clang-format, clang-tidy, AddressSanitizer / UBSan |
| Packaging | CPack (`.deb`, `.rpm`, `.tar.gz`), systemd units, Docker |
| CI | GitHub Actions (GCC and Clang, Debug and Release) |

## Project structure

Entries marked *(planned)* don't exist yet.

```
linux_monitoring_tool/
├── CMakeLists.txt              # top-level project, options, add_subdirectory()
├── CMakePresets.json           # debug / release / asan presets
├── cmake/
│   ├── Dependencies.cmake      # FetchContent for all third-party libraries (pinned + SHA256)
│   ├── CompilerWarnings.cmake  # lmon_set_warnings(): -Wall -Wextra -Wpedantic -Werror ...
│   └── Sanitizers.cmake        # lmon_enable_sanitizers(): ASan / UBSan
├── include/lmon/               # public headers of lmon_core
│   ├── collector.hpp           # ICollector interface
│   ├── procfs.hpp              # ProcFs: /proc and /sys access under a configurable root
│   ├── strings.hpp             # trim, split_whitespace, parse_number
│   ├── metrics.hpp             # (planned) metric structs
│   └── config.hpp              # (planned) YAML config
├── src/core/                   # lmon_core implementation
│   ├── version.hpp.in          # generates lmon/version.hpp at configure time
│   ├── procfs.cpp
│   ├── strings.cpp
│   └── collectors/             # (planned) cpu.cpp, memory.cpp, disk.cpp, network.cpp, ...
├── apps/
│   ├── cli/                    # lmon: `info` today; `snapshot` and `top` in Phase 1
│   ├── agent/                  # (planned) lmon-agent
│   └── server/                 # (planned) lmon-server
├── web/                        # (planned) static dashboard served by lmon-server
├── tests/
│   ├── fixtures/proc/          # sample /proc files for deterministic tests
│   └── *_test.cpp              # GoogleTest suites
├── deploy/                     # (planned) systemd units, Docker
├── config/                     # (planned) lmon.example.yaml
├── .github/workflows/ci.yml    # build + test matrix (gcc/clang × debug/release/asan), format check
├── .clang-format
├── .clang-tidy
├── .gitattributes              # forces LF line endings
└── README.md
```

## Core design

**Collector interface.** Every metric source implements one interface, so adding a new metric means adding one class:

```cpp
namespace lmon {

class ICollector {
public:
    virtual ~ICollector() = default;
    virtual std::string_view name() const = 0;
    virtual nlohmann::json collect() = 0;   // called once per interval
};

} // namespace lmon
```

**Testable `/proc` access.** Collectors receive a root path (default `/`) instead of hard-coding `/proc`. Tests point them at `tests/fixtures/proc/`, so they run the same way on any machine.

**Rates from deltas.** CPU %, disk I/O and network throughput need two samples. Collectors keep the previous raw counters and compute `(current - previous) / elapsed_time`.

**Agent loop.** A `std::jthread` runs collection on a fixed interval using `std::stop_token`. SIGTERM/SIGINT trigger a clean shutdown. If the server is unreachable, samples go into a bounded in-memory queue (later, a file on disk) and are sent when it comes back.

**Ownership and safety.** RAII everywhere, `std::unique_ptr` for collectors, no raw `new`/`delete`, no exceptions crossing thread boundaries. Builds run warnings-as-errors, and CI runs the test suite under AddressSanitizer and UBSan.

**Least privilege.** The agent runs as a dedicated non-root user. Almost everything in `/proc` and `/sys` that it needs is world-readable.

## Roadmap

- [x] **Phase 0 – Build skeleton:** CMake project, presets, dependency fetching, warnings and sanitizers, clang-format/clang-tidy, a hello-world `lmon` binary, GoogleTest wired into CTest, GitHub Actions CI.
- [ ] **Phase 1 – Collectors and CLI:** `/proc` parsers and the CPU, memory, disk, network, process and system collectors with fixture-based tests; `lmon snapshot --json`; `lmon top` live terminal UI with FTXUI; YAML config.
- [ ] **Phase 2 – Agent and local alerts:** `lmon-agent` daemon with the collection loop and signal handling; threshold rules with duration and cooldown; webhook notifications; systemd unit.
- [ ] **Phase 3 – Server and storage:** `lmon-server` with `POST /api/v1/ingest` and query endpoints; SQLite storage with retention; per-host auth tokens; "host down" detection; agent offline buffering.
- [ ] **Phase 4 – Web dashboard:** static dashboard served by `lmon-server`: host overview, per-host charts, alert list.
- [ ] **Phase 5 – Hardening and release:** TLS, systemd service monitoring, Prometheus `/metrics` endpoint, CPack packages (`.deb`/`.rpm`), Docker images, performance profiling.

## Building

### Requirements

- Linux (or **WSL2** on Windows, see below)
- GCC 11+ or Clang 14+ (tested with GCC 15.2)
- CMake 3.25+
- Ninja (optional, for faster builds)
- OpenSSL and SQLite3 development packages (needed from Phase 3)

On Ubuntu / Debian:

```bash
sudo apt update
sudo apt install build-essential cmake git curl ninja-build libssl-dev libsqlite3-dev
```

Third-party libraries (CLI11, spdlog, nlohmann/json, GoogleTest; later FTXUI, yaml-cpp, cpp-httplib, SQLiteCpp) are downloaded automatically by CMake on the first configure. Each one is pinned to a release with a SHA256 hash.

The presets use CMake's default generator (Unix Makefiles). To use Ninja instead, run `export CMAKE_GENERATOR=Ninja` before the first configure.

### Build and test

```bash
git clone <repository-url>
cd linux_monitoring_tool

# configure + build (Debug)
cmake --preset debug
cmake --build --preset debug

# run tests
ctest --preset debug

# release build
cmake --preset release
cmake --build --preset release
```

Planned presets:

| Preset | Purpose |
|---|---|
| `debug` | Debug symbols, no optimization |
| `release` | Optimized build (`-O2`) for deployment |
| `asan` | Debug build with AddressSanitizer + UBSan |

### Running

Available now:

```bash
./build/debug/apps/cli/lmon --help
./build/debug/apps/cli/lmon --version
./build/debug/apps/cli/lmon info            # version, build type, compiler
./build/debug/apps/cli/lmon info --json
```

Planned:

```bash
# one snapshot of all metrics as JSON
./build/debug/apps/cli/lmon snapshot --json

# live terminal dashboard
./build/debug/apps/cli/lmon top

# agent and server
./build/debug/apps/server/lmon-server --config config/lmon.example.yaml
./build/debug/apps/agent/lmon-agent  --config config/lmon.example.yaml
```

### Example configuration (planned, `config/lmon.example.yaml`)

```yaml
interval_seconds: 5

collectors:
  cpu: true
  memory: true
  disk: true
  network: true
  processes: true

agent:
  server_url: http://localhost:8080
  token: change-me

server:
  listen: 0.0.0.0:8080
  database: /var/lib/lmon/lmon.db
  retention_days: 7

alerts:
  - metric: cpu.usage_percent
    above: 90
    for: 5m
  - metric: disk.usage_percent
    above: 85
    for: 10m
```

## Developing on Windows

The tool reads `/proc` and `/sys`, which exist only on Linux, and it uses POSIX APIs. On Windows:

1. Install WSL2 with Ubuntu: `wsl --install -d Ubuntu`
2. Inside Ubuntu, install the packages listed under **Requirements**.
3. Building from the Windows drive (`/mnt/c/...`) works. When the build folder is on `/mnt/<drive>/`, [Dependencies.cmake](cmake/Dependencies.cmake) extracts the third-party libraries into `~/.cache/lmon/deps/` inside WSL instead. Extracting them on the Windows drive fails with `file RENAME failed to rename ... Permission denied`, because Windows Defender locks the freshly extracted files. For faster builds, keep the project inside the WSL filesystem (e.g. `~/projects/linux_monitoring_tool`); file access on `/mnt/c` is several times slower.
4. Open the folder with VS Code's **WSL** extension (`code .` from the WSL terminal).
5. Build and run with the CMake commands above, from the WSL terminal.

## Coding conventions

- C++20, formatted with `.clang-format`, checked with `.clang-tidy`.
- Code lives in the `lmon` namespace; headers use `#pragma once`.
- Files use `snake_case` (`cpu_collector.cpp`), types use `PascalCase`, functions and variables use `snake_case`.
- Every collector has fixture-based tests in `tests/`.

## Contributing

1. Create a branch for your change.
2. Add or update tests in `tests/`.
3. Build with the `asan` preset and run `ctest`. Everything must pass with no sanitizer errors.
4. Run clang-format before committing.
5. Open a pull request describing what changed and why.

## License

To be decided.
"# Lightweight_Linux_Monitoring_Tool" 
