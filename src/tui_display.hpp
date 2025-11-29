#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "log_reader.hpp"
#include "filter_engine.hpp"
#include "syntax_highlighter.hpp"

class TuiDisplay {
public:
    TuiDisplay(std::shared_ptr<LogReader> reader,
               std::shared_ptr<FilterEngine> filter,
               std::shared_ptr<SyntaxHighlighter> highlighter);
    ~TuiDisplay();

    // Run the TUI
    void run();

    // Stop the TUI
    void stop();

private:
    // Build UI components
    ftxui::Component buildUI();

    // Update visible lines based on current filter
    void updateVisibleLines();

    // Render a single line
    ftxui::Element renderLine(size_t visible_index);

    // Apply filter asynchronously
    void applyFilterAsync();

    // Handle key events
    bool onEvent(ftxui::Event event);

    std::shared_ptr<LogReader> reader_;
    std::shared_ptr<FilterEngine> filter_;
    std::shared_ptr<SyntaxHighlighter> highlighter_;

    // UI state
    std::string filter_input_;
    std::string status_message_;
    int scroll_position_;
    int selected_line_;
    bool highlight_enabled_;
    bool case_sensitive_;

    // Visible lines after filtering
    std::vector<size_t> visible_line_indices_;
    std::mutex visible_lines_mutex_;
    std::atomic<bool> filter_in_progress_;
    std::atomic<bool> should_exit_;

    // Screen
    ftxui::ScreenInteractive screen_;

    // UI Components
    ftxui::Component main_container_;
    ftxui::Component filter_input_component_;
    ftxui::Component log_display_component_;
};
