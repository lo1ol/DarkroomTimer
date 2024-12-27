#!/usr/bin/env bash

clang-format-17 -i $(find . -name "*.cpp" -o -name "*.h" -o -name "*.ino")
