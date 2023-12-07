#!/usr/bin/env bash

PI_BOARD_DEV=/dev/cu.usbmodem141101

## -> not needed if UART is used

MINICOM_ID=$(pgrep minicom)

if [ $? -eq 0 ]; then
  echo Minicom running, attempting to kill it.
  killall minicom
  if $(pgrep minicom); then
    sleep 1
  fi
fi

echo 'Setting board into bootsel mode'
stty -f $PI_BOARD_DEV 1200
sleep 5  # wait for the board to appear

echo 'flashing the board'
cp "$1" /Volumes/RPI-RP2/

echo "Done."
