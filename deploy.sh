#!/usr/bin/env bash

PI_BOARD_DEV="$2"

echo 'Setting board into bootsel mode'
stty -f "$PI_BOARD_DEV" 1200
sleep 5  # wait for the board to appear

echo 'flashing the board'
cp -X "$1" /Volumes/RPI-RP2/

echo "Done."
