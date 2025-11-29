#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include "log_reader.hpp"
#include "filter_engine.hpp"
#include "syntax_highlighter.hpp"
#include "tui_display.hpp"

void printUsage(const char* program_name) {
    std::cout << "Log Analyzer - High-Performance TUI Log Viewer\n\n";
    std::cout << "Usage: " << program_name << " <log_file>\n\n";
    std::cout << "Description:\n";
    std::cout << "  A fast terminal-based log analyzer for large files (up to 50+ GB)\n";
    std::cout << "  Uses memory-mapped files for instant loading and regex filtering\n\n";
    std::cout << "Features:\n";
    std::cout << "  - Instant opening of large files using mmap\n";
    std::cout << "  - Real-time regex filtering\n";
    std::cout << "  - Syntax highlighting for JSON/SQL\n";
    std::cout << "  - Smooth scrolling and navigation\n\n";
    std::cout << "Keyboard Controls:\n";
    std::cout << "  ↑/↓          Navigate lines\n";
    std::cout << "  PgUp/PgDn    Scroll page\n";
    std::cout << "  Home/End     Jump to start/end\n";
    std::cout << "  H            Toggle syntax highlighting\n";
    std::cout << "  Q/Esc        Quit\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " /var/log/app.log\n";
    std::cout << "  " << program_name << " large_file.log\n\n";
}

void printError(const std::string& message) {
    std::cerr << "Error: " << message << "\n";
    std::cerr << "Use --help for usage information.\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        printError("No log file specified");
        printUsage(argv[0]);
        return 1;
    }

    // Check for help flag
    if (argc == 2 && (std::strcmp(argv[1], "--help") == 0 ||
                       std::strcmp(argv[1], "-h") == 0)) {
        printUsage(argv[0]);
        return 0;
    }

    std::string log_file = argv[1];

    // Initialize components
    std::cout << "Log Analyzer v1.0\n";
    std::cout << "Loading file: " << log_file << "\n";

    auto reader = std::make_shared<LogReader>();
    if (!reader->open(log_file)) {
        printError("Failed to open log file: " + log_file);
        return 1;
    }

    std::cout << "File loaded successfully!\n";
    std::cout << "Total lines: " << reader->getLineCount() << "\n";
    std::cout << "File size: " << (reader->getFileSize() / 1024.0 / 1024.0) << " MB\n";
    std::cout << "\nStarting TUI...\n";

    // Small delay to let user see the info
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto filter = std::make_shared<FilterEngine>();
    auto highlighter = std::make_shared<SyntaxHighlighter>();

    try {
        // Create and run TUI
        TuiDisplay display(reader, filter, highlighter);
        display.run();
    } catch (const std::exception& e) {
        std::cerr << "\nFatal error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\nThank you for using Log Analyzer!\n";
    return 0;
}
