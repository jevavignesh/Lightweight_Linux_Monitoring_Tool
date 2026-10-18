add_executable(lmon
    main.cpp)

target_link_libraries(lmon
    PRIVATE
        lmon::core
        CLI11::CLI11
        spdlog::spdlog)

lmon_set_warnings(lmon)
lmon_enable_sanitizers(lmon)
