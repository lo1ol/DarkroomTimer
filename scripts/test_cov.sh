#!/usr/bin/env bash

set -eux

OUT_DIR=$1

SCRIPT_DIR=$(realpath $(dirname "$0"))
PROJ_DIR=$(dirname "$SCRIPT_DIR")

lcov --zerocounters -d "$PROJ_DIR" ||:
platformio test -d "$PROJ_DIR" -e native-cov
lcov -d "$PROJ_DIR" -c -o cov.info
lcov --remove cov.info "*/.pio/*" "/usr/*" "/test/" -o cov.info
genhtml cov.info --output-directory "$OUT_DIR"
