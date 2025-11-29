#include <gtest/gtest.h>
#include "../src/syntax_highlighter.hpp"
#include <string_view>

class SyntaxHighlighterTest : public ::testing::Test {
protected:
    void SetUp() override {
        highlighter_ = std::make_unique<SyntaxHighlighter>();
    }

    std::unique_ptr<SyntaxHighlighter> highlighter_;
};

TEST_F(SyntaxHighlighterTest, InitialState) {
    EXPECT_TRUE(highlighter_->isEnabled());
}

TEST_F(SyntaxHighlighterTest, EnableDisable) {
    highlighter_->setEnabled(false);
    EXPECT_FALSE(highlighter_->isEnabled());

    highlighter_->setEnabled(true);
    EXPECT_TRUE(highlighter_->isEnabled());
}

TEST_F(SyntaxHighlighterTest, HighlightPlainText) {
    std::string_view line = "This is plain text";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightSQLKeywords) {
    std::string_view line = "SELECT * FROM users WHERE id = 1";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightJSONKeywords) {
    std::string_view line = R"({"name": "test", "value": 123, "active": true})";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightNumbers) {
    std::string_view line = "Port: 8080, Timeout: 3000ms";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightStrings) {
    std::string_view line = R"(Message: "Hello, World!")";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightEmptyLine) {
    std::string_view line = "";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightMixedContent) {
    std::string_view line = R"(ERROR: Query failed: SELECT count FROM table WHERE status = "active")";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, DisabledHighlighting) {
    highlighter_->setEnabled(false);
    std::string_view line = "SELECT * FROM users";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightSpecialCharacters) {
    std::string_view line = "Status: [OK] | Response: {success}";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightLongLine) {
    std::string long_line(1000, 'x');
    auto element = highlighter_->highlight(long_line);
    EXPECT_NE(element, nullptr);
}

TEST_F(SyntaxHighlighterTest, HighlightUnicodeCharacters) {
    std::string_view line = "Пользователь: Тест, Status: ✓ OK";
    auto element = highlighter_->highlight(line);
    EXPECT_NE(element, nullptr);
}
