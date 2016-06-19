#!/bin/bash

PATH=/bin:/sbin:/usr/bin:/usr/sbin; export PATH

# --------------------------------------------------
sudo kextunload -b org.pqrs.mykext

exit 0
