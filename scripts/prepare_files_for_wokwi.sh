#!/usr/bin/env bash

set -eu

SCRIPT_DIR=$(realpath $(dirname "$0"))
SRC_DIR=$(dirname "$SCRIPT_DIR")/DarkroomTimer
WOKWI_DIR=${PWD}/wokwi

rm -rf "$WOKWI_DIR"
mkdir -p "$WOKWI_DIR"

find "$SRC_DIR" -type f | xargs -I{} cp {} "$WOKWI_DIR"
sed -I '' -e 's/#include "Modes\//#include "/' \
    -I '' -e 's/#include "LcdImpl\//#include "/' \
    -e 's/#include "..\//#include "/' \
    -e 's/gAnalogWrite(LCD_BACKLIGHT_PIN, val \* LCD_BACKLIGHT_STEP);/gAnalogWrite(LCD_BACKLIGHT_PIN, 255);/' \
    -e 's/#define BEEPER_VOLUME_SILENT .*/#define BEEPER_VOLUME_SILENT 0/'\
    -e 's/#define ENCODER_FAST_TIMEOUT .*/#define ENCODER_FAST_TIMEOUT 200/'\
    -e 's/#define ENCODER_FAST_FAST_TIMEOUT .*/#define ENCODER_FAST_FAST_TIMEOUT 100/'\
    $(find "$WOKWI_DIR"  -type f)

cp "$SCRIPT_DIR/wokwi_diagram.json" "$WOKWI_DIR/diagram.json"
cp "$SCRIPT_DIR/wokwi_libraries.txt" "$WOKWI_DIR/libraries.txt"
