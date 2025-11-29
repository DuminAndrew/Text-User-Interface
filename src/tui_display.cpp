#include "tui_display.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace ftxui;

TuiDisplay::TuiDisplay(std::shared_ptr<LogReader> reader,
                       std::shared_ptr<FilterEngine> filter,
                       std::shared_ptr<SyntaxHighlighter> highlighter)
    : reader_(reader)
    , filter_(filter)
    , highlighter_(highlighter)
    , scroll_position_(0)
    , selected_line_(0)
    , highlight_enabled_(true)
    , case_sensitive_(false)
    , filter_in_progress_(false)
    , should_exit_(false)
    , screen_(ScreenInteractive::Fullscreen()) {

    // Initialize with all lines visible
    updateVisibleLines();
}

TuiDisplay::~TuiDisplay() {
    stop();
}

void TuiDisplay::stop() {
    should_exit_ = true;
    screen_.Exit();
}

void TuiDisplay::run() {
    main_container_ = buildUI();
    screen_.Loop(main_container_);
}

Component TuiDisplay::buildUI() {
    // Filter input component
    InputOption input_option;
    input_option.on_change = [this]() {
        applyFilterAsync();
    };

    filter_input_component_ = Input(&filter_input_, "Enter regex pattern...", input_option);

    // Main component with custom renderer
    auto main_component = Renderer(filter_input_component_, [this] {
        std::lock_guard<std::mutex> lock(visible_lines_mutex_);

        // Header
        auto title = text("Log Analyzer") | bold | color(Color::Cyan);
        auto file_info = text(" File: " + reader_->getFilename());

        std::stringstream info;
        info << " Lines: " << visible_line_indices_.size()
             << "/" << reader_->getLineCount()
             << " Size: " << (reader_->getFileSize() / 1024 / 1024) << " MB";

        auto stats = text(info.str()) | color(Color::Yellow);

        Elements header_elements;
        header_elements.push_back(title);
        header_elements.push_back(file_info | flex);
        header_elements.push_back(stats);
        auto header = hbox(header_elements);

        // Filter input area
        Elements filter_box_elements;
        filter_box_elements.push_back(text(" Filter: ") | bold);
        filter_box_elements.push_back(filter_input_component_->Render() | flex | border);
        auto filter_box = hbox(filter_box_elements);

        // Log display area
        Elements lines_elements;
        int terminal_height = screen_.dimy() - 8;  // Reserve space for header/footer

        // Calculate visible range
        size_t start = scroll_position_;
        size_t end = std::min(start + static_cast<size_t>(terminal_height),
                              visible_line_indices_.size());

        for (size_t i = start; i < end; ++i) {
            size_t line_idx = visible_line_indices_[i];
            auto line_view = reader_->getLine(line_idx);

            // Line number
            std::stringstream ss;
            ss << std::setw(8) << std::right << (line_idx + 1) << " │ ";

            Element line_num = text(ss.str()) | color(Color::GreenLight);

            // Line content
            Element content;
            if (highlight_enabled_) {
                content = highlighter_->highlight(line_view);
            } else {
                content = text(std::string(line_view));
            }

            // Highlight selected line
            Elements line_parts;
            line_parts.push_back(line_num);
            line_parts.push_back(content);
            auto line_element = hbox(line_parts);

            if (i == static_cast<size_t>(selected_line_ + scroll_position_)) {
                line_element = line_element | bgcolor(Color::Blue) | bold;
            }

            lines_elements.push_back(line_element);
        }

        if (lines_elements.empty()) {
            lines_elements.push_back(text("No matching lines") | color(Color::Red) | center);
        }

        auto log_area = vbox(lines_elements) | flex;

        // Status bar
        std::string status = status_message_;
        if (filter_in_progress_) {
            status = "Filtering...";
        }

        Elements status_bar_elements;
        status_bar_elements.push_back(text(" " + status) | color(Color::GreenLight));
        status_bar_elements.push_back(filler());
        status_bar_elements.push_back(text(highlight_enabled_ ?
            " [H]ighlight: ON " : " [H]ighlight: OFF "));
        status_bar_elements.push_back(text(" [Q]uit "));
        auto status_bar = hbox(status_bar_elements);

        // Help bar
        auto help = text(" ↑↓: Navigate  PgUp/PgDn: Scroll  H: Toggle highlight  Q: Quit ") |
                    color(Color::GrayDark);

        // Main layout
        Elements main_layout;
        main_layout.push_back(header | border);
        main_layout.push_back(separator());
        main_layout.push_back(filter_box);
        main_layout.push_back(separator());
        main_layout.push_back(log_area);
        main_layout.push_back(separator());
        main_layout.push_back(status_bar | border);
        main_layout.push_back(help);

        return vbox(main_layout);
    });

    // Handle keyboard events
    main_component = CatchEvent(main_component, [this](Event event) {
        return onEvent(event);
    });

    return main_component;
}

bool TuiDisplay::onEvent(Event event) {
    int terminal_height = screen_.dimy() - 8;

    if (event == Event::ArrowUp) {
        if (selected_line_ > 0) {
            selected_line_--;
        } else if (scroll_position_ > 0) {
            scroll_position_--;
        }
        return true;
    }

    if (event == Event::ArrowDown) {
        std::lock_guard<std::mutex> lock(visible_lines_mutex_);
        if (selected_line_ < terminal_height - 1 &&
            scroll_position_ + selected_line_ + 1 < static_cast<int>(visible_line_indices_.size())) {
            selected_line_++;
        } else if (scroll_position_ + terminal_height < static_cast<int>(visible_line_indices_.size())) {
            scroll_position_++;
        }
        return true;
    }

    if (event == Event::PageUp) {
        scroll_position_ = std::max(0, scroll_position_ - terminal_height);
        return true;
    }

    if (event == Event::PageDown) {
        std::lock_guard<std::mutex> lock(visible_lines_mutex_);
        scroll_position_ = std::min(
            static_cast<int>(visible_line_indices_.size()) - terminal_height,
            scroll_position_ + terminal_height
        );
        scroll_position_ = std::max(0, scroll_position_);
        return true;
    }

    if (event == Event::Home) {
        scroll_position_ = 0;
        selected_line_ = 0;
        return true;
    }

    if (event == Event::End) {
        std::lock_guard<std::mutex> lock(visible_lines_mutex_);
        scroll_position_ = std::max(0,
            static_cast<int>(visible_line_indices_.size()) - terminal_height);
        return true;
    }

    if (event == Event::Character('h') || event == Event::Character('H')) {
        highlight_enabled_ = !highlight_enabled_;
        status_message_ = highlight_enabled_ ?
            "Syntax highlighting enabled" : "Syntax highlighting disabled";
        return true;
    }

    if (event == Event::Character('q') || event == Event::Character('Q')) {
        stop();
        return true;
    }

    if (event == Event::Escape) {
        stop();
        return true;
    }

    return false;
}

void TuiDisplay::updateVisibleLines() {
    std::lock_guard<std::mutex> lock(visible_lines_mutex_);

    visible_line_indices_.clear();
    visible_line_indices_.reserve(reader_->getLineCount());

    for (size_t i = 0; i < reader_->getLineCount(); ++i) {
        visible_line_indices_.push_back(i);
    }

    // Reset scroll position
    scroll_position_ = 0;
    selected_line_ = 0;

    status_message_ = "Showing all lines";
}

void TuiDisplay::applyFilterAsync() {
    if (filter_in_progress_) {
        return;  // Already filtering
    }

    std::string pattern = filter_input_;

    if (pattern.empty()) {
        updateVisibleLines();
        return;
    }

    filter_in_progress_ = true;

    // Launch async filter
    std::thread([this, pattern]() {
        // Set pattern
        if (!filter_->setPattern(pattern)) {
            status_message_ = "Invalid regex: " + filter_->getError();
            filter_in_progress_ = false;
            return;
        }

        // Get all lines
        auto all_lines = reader_->getLines(0, reader_->getLineCount());

        // Filter
        auto matching_indices = filter_->filter(all_lines);

        // Update visible lines
        {
            std::lock_guard<std::mutex> lock(visible_lines_mutex_);
            visible_line_indices_ = matching_indices;
            scroll_position_ = 0;
            selected_line_ = 0;
        }

        std::stringstream ss;
        ss << "Found " << matching_indices.size() << " matching lines";
        status_message_ = ss.str();

        filter_in_progress_ = false;
    }).detach();
}

Element TuiDisplay::renderLine(size_t visible_index) {
    if (visible_index >= visible_line_indices_.size()) {
        return text("");
    }

    size_t line_idx = visible_line_indices_[visible_index];
    auto line_view = reader_->getLine(line_idx);

    if (highlight_enabled_) {
        return highlighter_->highlight(line_view);
    } else {
        return text(std::string(line_view));
    }
}
