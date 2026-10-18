#include "lmon/collector.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

namespace {

class CountingCollector final : public lmon::ICollector {
public:
    [[nodiscard]] std::string_view name() const override { return "counter"; }

    [[nodiscard]] nlohmann::json collect() override { return {{"calls", ++calls_}}; }

private:
    int calls_ = 0;
};

TEST(Collector, UsableThroughInterface) {
    std::vector<std::unique_ptr<lmon::ICollector>> collectors;
    collectors.push_back(std::make_unique<CountingCollector>());

    auto& collector = *collectors.front();
    EXPECT_EQ(collector.name(), "counter");
    EXPECT_EQ(collector.collect()["calls"], 1);
    EXPECT_EQ(collector.collect()["calls"], 2);
}

} // namespace
