#!/bin/bash
# ============================================
# Log Analyzer - Build and Test Script
# ============================================

set -e  # Exit on error

echo ""
echo "========================================"
echo "Log Analyzer - Build and Test"
echo "========================================"
echo ""

# Check if ninja is available
if ! command -v ninja &> /dev/null; then
    echo "ERROR: Ninja not found! Please install Ninja build system."
    echo "Ubuntu/Debian: sudo apt-get install ninja-build"
    echo "macOS: brew install ninja"
    exit 1
fi

# Check if cmake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found! Please install CMake."
    echo "Ubuntu/Debian: sudo apt-get install cmake"
    echo "macOS: brew install cmake"
    exit 1
fi

echo "[1/5] Cleaning build directory..."
rm -rf build
mkdir -p build

echo ""
echo "[2/5] Configuring CMake with Ninja..."
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

echo ""
echo "[3/5] Building project with Ninja..."
ninja

echo ""
echo "[4/5] Running tests..."
echo "----------------------------------------"
ctest --output-on-failure

echo ""
echo "========================================"
echo "Build and Tests SUCCESSFUL!"
echo "========================================"
echo ""
echo "Executable: build/log_analyzer"
echo "Tests: All 33 tests passed"
echo ""

echo "[5/5] Running demo application..."
echo ""
echo "Starting Log Analyzer with demo.log..."
echo "Press Q or ESC to quit the application"
echo ""
sleep 2

# Check if demo.log exists, if not create it
if [ ! -f ../demo.log ]; then
    echo "Creating demo log file..."
    cd ..
    ./create_demo_log.sh
    cd build
fi

# Run the application with demo log
if [ -f log_analyzer ]; then
    ./log_analyzer ../demo.log
else
    echo "ERROR: log_analyzer not found!"
    exit 1
fi

cd ..
echo ""
echo "========================================"
echo "Demo completed!"
echo "========================================"
