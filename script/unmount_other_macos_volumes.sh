#!/bin/sh

for v in $(find /Volumes/macos_* -type d -maxdepth 1); do
  diskutil unmount "$v"
done
