#!/bin/sh

find /Volumes/macos_* -type d -maxdepth 0 -print 0 | xargs -0 diskutil unmount
