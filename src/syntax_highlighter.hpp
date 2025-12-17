#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include "ftxui/dom/elements.hpp"

class SyntaxHighlighter {
public:
    SyntaxHighlighter();

    // Highlight a line and return FTXUI element
    ftxui::Element highlight(std::string_view line);

    // Enable/disable highlighting
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

private:
    struct Token {
        enum class Type {
            Normal,
            Keyword,
            String,
            Number,
            Special,
            LogLevel,      // ERROR, WARN, INFO, DEBUG
            IPAddress,     // IP addresses
            Protocol,      // TCP, UDP, HTTP, etc.
            Timestamp      // Timestamps
        };

        Type type;
        std::string text;
    };

    std::vector<Token> tokenize(std::string_view line);
    ftxui::Element tokenToElement(const Token& token);
    bool isKeyword(const std::string& word) const;
    bool isNumber(const std::string& word) const;
    bool isLogLevel(const std::string& word) const;
    bool isIPAddress(const std::string& word) const;
    bool isProtocol(const std::string& word) const;

    std::unordered_set<std::string> sql_keywords_;
    std::unordered_set<std::string> json_keywords_;
    std::unordered_set<std::string> log_levels_;
    std::unordered_set<std::string> protocols_;
    bool enabled_;
};
