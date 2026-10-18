add_executable(lmon_tests
    collector_test.cpp
    procfs_test.cpp
    strings_test.cpp)

target_link_libraries(lmon_tests
    PRIVATE
        lmon::core
        GTest::gtest_main)

target_compile_definitions(lmon_tests
    PRIVATE
        LMON_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

lmon_set_warnings(lmon_tests)
lmon_enable_sanitizers(lmon_tests)

include(GoogleTest)
gtest_discover_tests(lmon_tests)
