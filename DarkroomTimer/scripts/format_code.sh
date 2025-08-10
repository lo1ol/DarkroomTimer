#!/usr/bin/env bash

SCRIPT_DIR=$(realpath $(dirname "$0"))
clang-format-17 -i $(find "$SCRIPT_DIR/.." -name "*.cpp" -o -name "*.h" -o -name "*.ino")
