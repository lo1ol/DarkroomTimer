#!/usr/bin/env bash

SCRIPT_DIR=$(realpath $(dirname "$0"))
pushd "$SCRIPT_DIR/.."  > /dev/null

rm -rf ./Lightmeter/src/common/

pushd ./DarkroomTimer/src/common/ > /dev/null
files=$(find . -type f)
popd  > /dev/null

for file in $files; do
    mkdir -p "$(dirname "./Lightmeter/src/common/$file")"
    ln "./DarkroomTimer/src/common/$file" "./Lightmeter/src/common/$file"
done
