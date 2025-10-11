#!/usr/bin/env bash

SCRIPT_DIR=$(realpath $(dirname "$0"))
cd "$SCRIPT_DIR/.."
clang-format-17 -i $(find . -name "*.cpp" -o -name "*.h" -o -name "*.ino")
