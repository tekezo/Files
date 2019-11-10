#!/bin/sh

find /Volumes/macos_* -type d -maxdepth 0 -print0 | xargs -0 diskutil unmount
