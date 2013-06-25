#!/bin/bash -e
make clean all
cp -r src build
mkdir -p release
cd build
tar cz asr15.elf asr15.bin asr15.hex asr15.lst src > ../release/$(git describe --tags).tgz
rm -rf src

