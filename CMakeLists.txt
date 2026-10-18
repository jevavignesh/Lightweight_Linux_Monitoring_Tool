cmake_minimum_required(VERSION 3.25)

project(lmon
    VERSION 0.1.0
    DESCRIPTION "Lightweight Linux monitoring tool"
    LANGUAGES CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "lmon reads /proc and /sys and only builds on Linux (use WSL2 on Windows).")
endif()

# ---- Options ---------------------------------------------------------------

option(LMON_BUILD_TESTS        "Build the unit tests"                    ON)
option(LMON_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"       ON)
option(LMON_ENABLE_ASAN        "Build with AddressSanitizer"             OFF)
option(LMON_ENABLE_UBSAN       "Build with UndefinedBehaviorSanitizer"   OFF)

# ---- Global settings -------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CompilerWarnings)
include(Sanitizers)
include(Dependencies)

# ---- Targets ---------------------------------------------------------------

add_subdirectory(src/core)
add_subdirectory(apps)

if(LMON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
