#include "filter_engine.hpp"
#include <algorithm>
#include <execution>

FilterEngine::FilterEngine()
    : has_valid_pattern_(false) {
}

FilterEngine::~FilterEngine() = default;

bool FilterEngine::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(mutex_);

    pattern_ = pattern;
    error_message_.clear();

    if (pattern.empty()) {
        has_valid_pattern_ = false;
        return true;
    }

    try {
        regex_ = std::regex(pattern,
            std::regex_constants::ECMAScript |
            std::regex_constants::optimize);
        has_valid_pattern_ = true;
        return true;
    } catch (const std::regex_error& e) {
        error_message_ = std::string("Regex error: ") + e.what();
        has_valid_pattern_ = false;
        return false;
    }
}

void FilterEngine::clearPattern() {
    std::lock_guard<std::mutex> lock(mutex_);
    pattern_.clear();
    has_valid_pattern_ = false;
    error_message_.clear();
}

std::vector<size_t> FilterEngine::filter(const std::vector<std::string_view>& lines) {
    return filterImpl(lines);
}

std::future<std::vector<size_t>> FilterEngine::filterAsync(
    const std::vector<std::string_view>& lines) {

    return std::async(std::launch::async, [this, lines]() {
        return filterImpl(lines);
    });
}

std::vector<size_t> FilterEngine::filterImpl(const std::vector<std::string_view>& lines) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<size_t> matching_indices;

    // If no pattern, return all line indices
    if (!has_valid_pattern_) {
        matching_indices.resize(lines.size());
        for (size_t i = 0; i < lines.size(); ++i) {
            matching_indices[i] = i;
        }
        return matching_indices;
    }

    // Reserve space (assume ~10% match rate)
    matching_indices.reserve(lines.size() / 10);

    // Filter lines using regex
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];

        // Convert string_view to string for regex matching
        std::string line_str(line);

        try {
            if (std::regex_search(line_str, regex_)) {
                matching_indices.push_back(i);
            }
        } catch (const std::regex_error&) {
            // Skip lines that cause regex errors
            continue;
        }
    }

    return matching_indices;
}
