#include "syntax_highlighter.hpp"
#include "ftxui/dom/elements.hpp"
#include <cctype>
#include <algorithm>

using namespace ftxui;

SyntaxHighlighter::SyntaxHighlighter()
    : enabled_(true) {

    // SQL Keywords (расширенный список)
    sql_keywords_ = {
        "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE",
        "CREATE", "DROP", "ALTER", "TABLE", "INDEX", "VIEW",
        "JOIN", "LEFT", "RIGHT", "INNER", "OUTER", "ON",
        "AND", "OR", "NOT", "IN", "LIKE", "BETWEEN",
        "ORDER", "BY", "GROUP", "HAVING", "LIMIT", "OFFSET",
        "AS", "DISTINCT", "COUNT", "SUM", "AVG", "MAX", "MIN",
        "NULL", "TRUE", "FALSE", "IS", "EXISTS",
        // Дополнительные SQL ключевые слова
        "BEGIN", "COMMIT", "ROLLBACK", "TRANSACTION",
        "PRIMARY", "KEY", "FOREIGN", "REFERENCES", "CONSTRAINT",
        "UNIQUE", "CHECK", "DEFAULT", "AUTO_INCREMENT",
        "VARCHAR", "INT", "INTEGER", "TEXT", "BLOB", "DATE", "DATETIME",
        "CASE", "WHEN", "THEN", "ELSE", "END",
        "UNION", "ALL", "INTERSECT", "EXCEPT",
        "GRANT", "REVOKE", "WITH", "USING"
    };

    // JSON Keywords
    json_keywords_ = {
        "true", "false", "null"
    };

    // Log Levels
    log_levels_ = {
        "ERROR", "FATAL", "CRITICAL", "CRIT",
        "WARN", "WARNING",
        "INFO", "INFORMATION",
        "DEBUG", "TRACE",
        "SUCCESS", "OK"
    };

    // Network Protocols (для Wireshark)
    protocols_ = {
        "TCP", "UDP", "HTTP", "HTTPS", "FTP", "SSH", "DNS", "DHCP",
        "SMTP", "POP3", "IMAP", "TLS", "SSL", "ICMP", "ARP",
        "IPv4", "IPv6", "ETHERNET", "WEBSOCKET", "MQTT", "AMQP"
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

    // Post-process to identify keywords, numbers, log levels, etc.
    for (auto& token : tokens) {
        if (token.type == Token::Type::Normal) {
            std::string upper = token.text;
            std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

            if (isLogLevel(upper)) {
                token.type = Token::Type::LogLevel;
                token.text = upper;  // Нормализуем к верхнему регистру
            } else if (isProtocol(upper)) {
                token.type = Token::Type::Protocol;
                token.text = upper;
            } else if (isIPAddress(token.text)) {
                token.type = Token::Type::IPAddress;
            } else if (isKeyword(token.text)) {
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
        case Token::Type::LogLevel:
            // Разные цвета для разных уровней логирования
            if (token.text == "ERROR" || token.text == "FATAL" || token.text == "CRITICAL" || token.text == "CRIT") {
                return text(token.text) | color(Color::Red) | bold;
            } else if (token.text == "WARN" || token.text == "WARNING") {
                return text(token.text) | color(Color::Yellow) | bold;
            } else if (token.text == "INFO" || token.text == "INFORMATION") {
                return text(token.text) | color(Color::Blue) | bold;
            } else if (token.text == "DEBUG" || token.text == "TRACE") {
                return text(token.text) | color(Color::GrayLight);
            } else if (token.text == "SUCCESS" || token.text == "OK") {
                return text(token.text) | color(Color::Green) | bold;
            }
            return text(token.text) | color(Color::White);
        case Token::Type::IPAddress:
            return text(token.text) | color(Color::CyanLight);
        case Token::Type::Protocol:
            return text(token.text) | color(Color::Magenta) | bold;
        case Token::Type::Timestamp:
            return text(token.text) | color(Color::GrayLight);
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

bool SyntaxHighlighter::isLogLevel(const std::string& word) const {
    return log_levels_.count(word) > 0;
}

bool SyntaxHighlighter::isProtocol(const std::string& word) const {
    return protocols_.count(word) > 0;
}

bool SyntaxHighlighter::isIPAddress(const std::string& word) const {
    // Простая проверка IPv4 адреса (xxx.xxx.xxx.xxx)
    if (word.empty()) return false;

    int dots = 0;
    int digits_in_group = 0;
    int current_value = 0;

    for (char c : word) {
        if (std::isdigit(c)) {
            current_value = current_value * 10 + (c - '0');
            digits_in_group++;
            if (current_value > 255 || digits_in_group > 3) {
                return false;
            }
        } else if (c == '.') {
            if (digits_in_group == 0 || digits_in_group > 3) {
                return false;
            }
            dots++;
            digits_in_group = 0;
            current_value = 0;
        } else if (c == ':') {
            // Может быть IPv6 или порт - считаем валидным
            return true;
        } else {
            return false;
        }
    }

    // IPv4: должно быть ровно 3 точки и последняя группа должна содержать цифры
    return (dots == 3 && digits_in_group > 0 && digits_in_group <= 3);
}
