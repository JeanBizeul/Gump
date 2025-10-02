#!/bin/bash
set -e  # exit on any error

# Paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/.."
BUILD_DIR="$PROJECT_DIR/build"

# Ensure build directory exists
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
cmake "$PROJECT_DIR" -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build .

# Ask user if they want to run the app
read -p "Do you want to run Gump directly? [Y/n] " runchoice
runchoice=${runchoice:-Y}  # default to Y if empty

if [[ "$runchoice" =~ ^[Yy]$ ]]; then
    echo "Launching Gump..."
    ./gump
else
    echo "Skipping launch."
fi
