#!/bin/sh

basedir=`dirname $0`
targetdir=/tmp/pseudo.$bundleidentifier.app

/bin/rm -rf $targetdir
rsync -a $basedir/pseudo.app/ $targetdir
/usr/bin/sed "s|org.pqrs.pseudo|$bundleidentifier|g" $targetdir/Contents/Info.plist > $targetdir/Contents/Info.plist.new
/bin/mv $targetdir/Contents/Info.plist.new $targetdir/Contents/Info.plist
open $targetdir
