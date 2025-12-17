cmake_minimum_required(VERSION 3.14)
project(LogAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === КРОССПЛАТФОРМЕННАЯ ОПТИМИЗАЦИЯ ===
# Настройки компиляции для разных платформ
if(MSVC)
    # Флаги для Microsoft Visual C++
    add_compile_options(/W4 /utf-8)
    # /W4 - высокий уровень предупреждений
    # /utf-8 - корректная работа с UTF-8
    # Используем RelWithDebInfo по умолчанию для оптимизации с отладочной информацией
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif()
else()
    # Флаги для GCC/Clang (Linux/macOS)
    add_compile_options(-Wall -Wextra -O3 -march=native)
    # -O3: максимальная оптимизация
    # -march=native: использовать все инструкции процессора (AVX и т.д.)
endif()

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

# Threading support (Обязательно для Linux)
find_package(Threads REQUIRED)
target_link_libraries(log_analyzer PRIVATE Threads::Threads)

# Installation
install(TARGETS log_analyzer DESTINATION bin)

# Enable testing
enable_testing()

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# Force GoogleTest to use shared runtime library on Windows (same as our project)
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

# Create a library from core components (without main.cpp)
add_library(log_analyzer_lib
    src/log_reader.cpp
    src/filter_engine.cpp
    src/syntax_highlighter.cpp
    src/tui_display.cpp
)

target_link_libraries(log_analyzer_lib
    PUBLIC ftxui::screen
    PUBLIC ftxui::dom
    PUBLIC ftxui::component
    PUBLIC Threads::Threads
)

# Tests
add_executable(log_analyzer_tests
    tests/test_log_reader.cpp
    tests/test_filter_engine.cpp
    tests/test_syntax_highlighter.cpp
)

target_link_libraries(log_analyzer_tests
    PRIVATE log_analyzer_lib
    PRIVATE GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(log_analyzer_tests)