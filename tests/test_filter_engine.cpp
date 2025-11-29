#include <gtest/gtest.h>
#include "../src/filter_engine.hpp"
#include <string_view>
#include <vector>

class FilterEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_lines_ = {
            "ERROR: Connection failed",
            "INFO: Server started successfully",
            "DEBUG: Processing request",
            "ERROR: Invalid input",
            "WARNING: Low memory",
            "INFO: Request completed"
        };
    }

    std::vector<std::string_view> test_lines_;
};

TEST_F(FilterEngineTest, SetValidPattern) {
    FilterEngine engine;
    EXPECT_TRUE(engine.setPattern("ERROR"));
    EXPECT_TRUE(engine.hasValidPattern());
}

TEST_F(FilterEngineTest, SetInvalidPattern) {
    FilterEngine engine;
    EXPECT_FALSE(engine.setPattern("[invalid(regex"));
    EXPECT_FALSE(engine.hasValidPattern());
    EXPECT_FALSE(engine.getError().empty());
}

TEST_F(FilterEngineTest, FilterSimplePattern) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("ERROR"));

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), 2);
    EXPECT_EQ(indices[0], 0); // "ERROR: Connection failed"
    EXPECT_EQ(indices[1], 3); // "ERROR: Invalid input"
}

TEST_F(FilterEngineTest, FilterRegexPattern) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("INFO.*success"));

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), 1);
    EXPECT_EQ(indices[0], 1); // "INFO: Server started successfully"
}

TEST_F(FilterEngineTest, FilterCaseInsensitive) {
    FilterEngine engine;
    // C++ std::regex with ECMAScript doesn't support (?i) modifier
    // Use character class pattern instead
    ASSERT_TRUE(engine.setPattern("[Ww][Aa][Rr][Nn][Ii][Nn][Gg]"));

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), 1);
    EXPECT_EQ(indices[0], 4); // "WARNING: Low memory"
}

TEST_F(FilterEngineTest, FilterNoMatches) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("CRITICAL"));

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), 0);
}

TEST_F(FilterEngineTest, FilterAllMatches) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern(".*")); // Match all

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), test_lines_.size());
}

TEST_F(FilterEngineTest, ClearPattern) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("ERROR"));
    EXPECT_TRUE(engine.hasValidPattern());

    engine.clearPattern();
    EXPECT_FALSE(engine.hasValidPattern());
    EXPECT_TRUE(engine.getPattern().empty());
}

TEST_F(FilterEngineTest, GetPattern) {
    FilterEngine engine;
    std::string pattern = "ERROR|WARNING";
    ASSERT_TRUE(engine.setPattern(pattern));
    EXPECT_EQ(engine.getPattern(), pattern);
}

TEST_F(FilterEngineTest, FilterEmptyLines) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("ERROR"));

    std::vector<std::string_view> empty_lines;
    auto indices = engine.filter(empty_lines);
    EXPECT_EQ(indices.size(), 0);
}

TEST_F(FilterEngineTest, FilterAsync) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("INFO"));

    auto future = engine.filterAsync(test_lines_);
    auto indices = future.get();

    EXPECT_EQ(indices.size(), 2);
    EXPECT_EQ(indices[0], 1); // "INFO: Server started successfully"
    EXPECT_EQ(indices[1], 5); // "INFO: Request completed"
}

TEST_F(FilterEngineTest, ComplexRegexPattern) {
    FilterEngine engine;
    ASSERT_TRUE(engine.setPattern("^(ERROR|WARNING):"));

    auto indices = engine.filter(test_lines_);
    EXPECT_EQ(indices.size(), 3);
    EXPECT_EQ(indices[0], 0); // ERROR: Connection failed
    EXPECT_EQ(indices[1], 3); // ERROR: Invalid input
    EXPECT_EQ(indices[2], 4); // WARNING: Low memory
}
