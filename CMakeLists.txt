cmake_minimum_required(VERSION 3.14)
project(LogAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

# Include FetchContent for external dependencies
include(FetchContent)

# Fetch FTXUI library
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(ftxui)

# Source files
set(SOURCES
    src/main.cpp
    src/log_reader.cpp
    src/filter_engine.cpp
    src/syntax_highlighter.cpp
    src/tui_display.cpp
)

# Headers
set(HEADERS
    src/log_reader.hpp
    src/filter_engine.hpp
    src/syntax_highlighter.hpp
    src/tui_display.hpp
)

# Create executable
add_executable(log_analyzer ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(log_analyzer
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(log_analyzer PRIVATE Threads::Threads)

# Installation
install(TARGETS log_analyzer DESTINATION bin)
