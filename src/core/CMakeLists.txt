configure_file(
    version.hpp.in
    "${PROJECT_BINARY_DIR}/generated/include/lmon/version.hpp"
    @ONLY)

add_library(lmon_core STATIC
    procfs.cpp
    strings.cpp)

add_library(lmon::core ALIAS lmon_core)

target_include_directories(lmon_core
    PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_BINARY_DIR}/generated/include")

target_link_libraries(lmon_core
    PUBLIC
        nlohmann_json::nlohmann_json)

lmon_set_warnings(lmon_core)
lmon_enable_sanitizers(lmon_core)
