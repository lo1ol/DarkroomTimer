#!/usr/bin/env bash

SCRIPT_DIR=$(realpath $(dirname "$0"))
cd "$SCRIPT_DIR/.."
clang-format-17 -i $(find test native_libs DarkroomTimer -name "*.cpp" -o -name "*.h" -o -name "*.ino")
