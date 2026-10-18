#include "lmon/procfs.hpp"

#include <gtest/gtest.h>

namespace {

lmon::ProcFs fixtures() {
    return lmon::ProcFs{LMON_FIXTURES_DIR};
}

TEST(ProcFs, ResolveIgnoresLeadingSlash) {
    const lmon::ProcFs fs{"/some/root"};
    EXPECT_EQ(fs.resolve("/proc/stat"), std::filesystem::path{"/some/root/proc/stat"});
    EXPECT_EQ(fs.resolve("proc/stat"), std::filesystem::path{"/some/root/proc/stat"});
}

TEST(ProcFs, ReadFileReturnsContents) {
    const auto contents = fixtures().read_file("/proc/uptime");
    ASSERT_TRUE(contents.has_value());
    EXPECT_EQ(*contents, "12345.67 98765.43\n");
}

TEST(ProcFs, ReadFileMissingReturnsNullopt) {
    EXPECT_FALSE(fixtures().read_file("/proc/does-not-exist").has_value());
}

TEST(ProcFs, ReadLinesSplitsOnNewlines) {
    const auto lines = fixtures().read_lines("/proc/meminfo");
    ASSERT_TRUE(lines.has_value());
    ASSERT_EQ(lines->size(), 3U);
    EXPECT_EQ((*lines)[0], "MemTotal:       16318400 kB");
    EXPECT_EQ((*lines)[2], "MemAvailable:   10485760 kB");
}

TEST(ProcFs, ReadsRealProcUptime) {
    // Sanity check against the live system rather than fixtures.
    const lmon::ProcFs fs;
    const auto contents = fs.read_file("/proc/uptime");
    ASSERT_TRUE(contents.has_value());
    EXPECT_FALSE(contents->empty());
}

} // namespace
