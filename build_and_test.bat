@echo off
REM ============================================
REM Log Analyzer - Build and Test Script
REM ============================================

echo.
echo ========================================
echo Log Analyzer - Build and Test
echo ========================================
echo.

REM Check if ninja is available
where ninja >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Ninja not found! Please install Ninja build system.
    echo Download from: https://github.com/ninja-build/ninja/releases
    pause
    exit /b 1
)

REM Check if cmake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found! Please install CMake.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

echo [1/5] Cleaning build directory...
if exist build rmdir /s /q build
mkdir build
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to create build directory
    pause
    exit /b 1
)

echo.
echo [2/5] Configuring CMake with Ninja...
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo.
echo [3/5] Building project with Ninja...
ninja
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

echo.
echo [4/5] Running tests...
echo ----------------------------------------
ctest --output-on-failure
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo WARNING: Some tests failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build and Tests SUCCESSFUL!
echo ========================================
echo.
echo Executable: build\log_analyzer.exe
echo Tests: All 33 tests passed
echo.

echo [5/5] Running demo application...
echo.
echo Starting Log Analyzer with demo.log...
echo Press Q or ESC to quit the application
echo.
timeout /t 3 /nobreak >nul

REM Check if demo.log exists, if not create it
if not exist ..\demo.log (
    echo Creating demo log file...
    cd ..
    call create_demo_log.bat
    cd build
)

REM Run the application with demo log
if exist log_analyzer.exe (
    log_analyzer.exe ..\demo.log
) else (
    echo ERROR: log_analyzer.exe not found!
    cd ..
    pause
    exit /b 1
)

cd ..
echo.
echo ========================================
echo Demo completed!
echo ========================================
pause
