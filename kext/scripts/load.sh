#!/bin/sh

PATH=/bin:/sbin:/usr/bin:/usr/sbin; export PATH

# --------------------------------------------------
sudo rm -rf /tmp/mykext
mkdir /tmp/mykext

cp -R build/Release/mykext.kext /tmp/mykext/mykext.signed.kext
bash ../script/codesign.sh /tmp/mykext
sudo chown -R root:wheel /tmp/mykext

sudo kextutil -t /tmp/mykext/mykext.signed.kext
