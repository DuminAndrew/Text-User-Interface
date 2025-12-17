#pragma once

#include <string>
#include <vector>
#include <regex>
#include <string_view>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

class FilterEngine {
public:
    FilterEngine();
    ~FilterEngine();

    // Set the regex pattern for filtering
    bool setPattern(const std::string& pattern);

    // Filter lines synchronously
    std::vector<size_t> filter(const std::vector<std::string_view>& lines);

    // Filter lines asynchronously (returns future with line indices)
    std::future<std::vector<size_t>> filterAsync(
        const std::vector<std::string_view>& lines
    );

    // Check if pattern is valid
    bool hasValidPattern() const { return has_valid_pattern_; }

    // Get current pattern
    const std::string& getPattern() const { return pattern_; }

    // Get last error message
    const std::string& getError() const { return error_message_; }

    // Clear pattern (show all lines)
    void clearPattern();

    // Check if a single line matches the current pattern
    bool matches(std::string_view line) const;

private:
    std::vector<size_t> filterImpl(const std::vector<std::string_view>& lines);

    std::string pattern_;
    std::regex regex_;
    bool has_valid_pattern_;
    std::string error_message_;
    std::mutex mutex_;
};
