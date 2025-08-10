#!/usr/bin/env bash

set -eu

SCRIPT_DIR=$(realpath $(dirname "$0"))
SRC_DIR=$(dirname "$SCRIPT_DIR")/src
WOKWI_DIR=${PWD}/wokwi

rm -rf "$WOKWI_DIR"
mkdir -p "$WOKWI_DIR"

find "$SRC_DIR" -type f | xargs -I{} cp {} "$WOKWI_DIR"
sed -I '' -e 's/#include "Modes\//#include "/' \
    -e 's/#include "..\//#include "/' \
    -e 's/gAnalogWrite(BACKLIGHT, val \* BACKLIGHT_STEP);/gAnalogWrite(BACKLIGHT, 255);/' \
    -e 's/#define BEEP_VOLUME_SILENT .*/#define BEEP_VOLUME_SILENT 0/'\
    $(find "$WOKWI_DIR"  -type f)

cp "$SCRIPT_DIR/wokwi_diagram.json" "$WOKWI_DIR/diagram.json"
cp "$SCRIPT_DIR/wokwi_libraries.txt" "$WOKWI_DIR/libraries.txt"
