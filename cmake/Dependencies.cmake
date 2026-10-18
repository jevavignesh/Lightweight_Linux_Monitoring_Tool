# Third-party dependencies, downloaded at configure time.
#
# Each one is pinned to a release tarball with a SHA256 hash, so builds are
# reproducible and don't need git. Later phases add FTXUI, yaml-cpp,
# cpp-httplib and SQLiteCpp here.

# When building on a Windows drive from WSL (/mnt/c/...), extracting the
# tarballs there often fails with "file RENAME failed to rename ... Permission
# denied": Windows Defender holds the freshly extracted files open. Extract
# into the Linux filesystem instead, one folder per build directory.
# Override with -DFETCHCONTENT_BASE_DIR=<path>.
if(NOT DEFINED FETCHCONTENT_BASE_DIR
        AND CMAKE_BINARY_DIR MATCHES "^/mnt/[a-z]/"
        AND DEFINED ENV{HOME})
    string(SHA1 build_dir_hash "${CMAKE_BINARY_DIR}")
    string(SUBSTRING "${build_dir_hash}" 0 12 build_dir_hash)
    set(FETCHCONTENT_BASE_DIR "$ENV{HOME}/.cache/lmon/deps/${build_dir_hash}")
    message(STATUS "Build directory is on a Windows drive; dependencies go to ${FETCHCONTENT_BASE_DIR}")
endif()

include(FetchContent)

set(FETCHCONTENT_QUIET ON)

# ---- CLI11: command-line parsing ------------------------------------------
FetchContent_Declare(CLI11
    URL      https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.5.0.tar.gz
    URL_HASH SHA256=17e02b4cddc2fa348e5dbdbb582c59a3486fa2b2433e70a0c3bacb871334fd55
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SYSTEM)

# ---- spdlog: logging (bundles fmt) ----------------------------------------
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
    URL      https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz
    URL_HASH SHA256=9962648c9b4f1a7bbc76fd8d9172555bad1871fdb14ff4f842ef87949682caa5
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SYSTEM)

# ---- nlohmann/json: JSON ---------------------------------------------------
FetchContent_Declare(nlohmann_json
    URL      https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SYSTEM)

FetchContent_MakeAvailable(CLI11 spdlog nlohmann_json)

# ---- GoogleTest: unit tests (only when tests are enabled) -----------------
if(LMON_BUILD_TESTS)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK   ON  CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest
        URL      https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SYSTEM)
    FetchContent_MakeAvailable(googletest)
endif()
