#include "syntax_highlighter.hpp"
#include "ftxui/dom/elements.hpp"
#include <cctype>
#include <algorithm>

using namespace ftxui;

SyntaxHighlighter::SyntaxHighlighter()
    : enabled_(true) {

    // SQL Keywords
    sql_keywords_ = {
        "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE",
        "CREATE", "DROP", "ALTER", "TABLE", "INDEX", "VIEW",
        "JOIN", "LEFT", "RIGHT", "INNER", "OUTER", "ON",
        "AND", "OR", "NOT", "IN", "LIKE", "BETWEEN",
        "ORDER", "BY", "GROUP", "HAVING", "LIMIT", "OFFSET",
        "AS", "DISTINCT", "COUNT", "SUM", "AVG", "MAX", "MIN",
        "NULL", "TRUE", "FALSE", "IS", "EXISTS"
    };

    // JSON Keywords
    json_keywords_ = {
        "true", "false", "null"
    };
}

ftxui::Element SyntaxHighlighter::highlight(std::string_view line) {
    if (!enabled_) {
        return text(std::string(line));
    }

    auto tokens = tokenize(line);

    if (tokens.empty()) {
        return text("");
    }

    // Build elements
    ftxui::Elements elements;
    for (const auto& token : tokens) {
        elements.push_back(tokenToElement(token));
    }

    return hbox(std::move(elements));
}

std::vector<SyntaxHighlighter::Token> SyntaxHighlighter::tokenize(std::string_view line) {
    std::vector<Token> tokens;
    std::string current;

    auto flush_current = [&](Token::Type type) {
        if (!current.empty()) {
            tokens.push_back({type, current});
            current.clear();
        }
    };

    bool in_string = false;
    char string_delimiter = '\0';

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        // Handle strings
        if (c == '"' || c == '\'') {
            if (!in_string) {
                flush_current(Token::Type::Normal);
                in_string = true;
                string_delimiter = c;
                current += c;
            } else if (c == string_delimiter) {
                current += c;
                flush_current(Token::Type::String);
                in_string = false;
                string_delimiter = '\0';
            } else {
                current += c;
            }
            continue;
        }

        if (in_string) {
            current += c;
            continue;
        }

        // Handle special characters
        if (c == '{' || c == '}' || c == '[' || c == ']' ||
            c == '(' || c == ')' || c == ',' || c == ':' ||
            c == ';' || c == '=') {
            flush_current(Token::Type::Normal);
            tokens.push_back({Token::Type::Special, std::string(1, c)});
            continue;
        }

        // Handle whitespace
        if (std::isspace(c)) {
            flush_current(Token::Type::Normal);
            current += c;
            flush_current(Token::Type::Normal);
            continue;
        }

        // Accumulate other characters
        current += c;
    }

    // Flush remaining
    if (in_string) {
        flush_current(Token::Type::String);
    } else {
        flush_current(Token::Type::Normal);
    }

    // Post-process to identify keywords and numbers
    for (auto& token : tokens) {
        if (token.type == Token::Type::Normal) {
            std::string upper = token.text;
            std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

            if (isKeyword(token.text)) {
                token.type = Token::Type::Keyword;
            } else if (isNumber(token.text)) {
                token.type = Token::Type::Number;
            }
        }
    }

    return tokens;
}

ftxui::Element SyntaxHighlighter::tokenToElement(const Token& token) {
    switch (token.type) {
        case Token::Type::Keyword:
            return text(token.text) | color(Color::Cyan) | bold;
        case Token::Type::String:
            return text(token.text) | color(Color::Green);
        case Token::Type::Number:
            return text(token.text) | color(Color::Magenta);
        case Token::Type::Special:
            return text(token.text) | color(Color::Yellow);
        case Token::Type::Normal:
        default:
            return text(token.text);
    }
}

bool SyntaxHighlighter::isKeyword(const std::string& word) const {
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    return sql_keywords_.count(upper) > 0 || json_keywords_.count(word) > 0;
}

bool SyntaxHighlighter::isNumber(const std::string& word) const {
    if (word.empty()) return false;

    bool has_digit = false;
    bool has_dot = false;

    for (size_t i = 0; i < word.size(); ++i) {
        char c = word[i];

        if (std::isdigit(c)) {
            has_digit = true;
        } else if (c == '.' && !has_dot) {
            has_dot = true;
        } else if (c == '-' && i == 0) {
            // Negative sign at start
            continue;
        } else {
            return false;
        }
    }

    return has_digit;
}
