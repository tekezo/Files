#!/bin/sh

PATH=/bin:/sbin:/usr/bin:/usr/sbin; export PATH

# --------------------------------------------------
targetdir=/tmp/org.pqrs.driver.VirtualHIDManager

sudo rm -rf $targetdir
mkdir $targetdir

cp -R build/Release/VirtualHIDManager.kext $targetdir/VirtualHIDManager.signed.kext
bash ../../script/codesign.sh $targetdir
sudo chown -R root:wheel $targetdir

sudo kextutil -t $targetdir/VirtualHIDManager.signed.kext
