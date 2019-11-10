#!/bin/sh

find /Volumes/macos_* -type d -maxdepth 0 | while read v; do
  diskutil unmount "$v"
done
