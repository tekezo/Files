#!/bin/bash

for f in *.rb; do
  outfile=$(basename $f .rb)
  ruby $f >$outfile
  '/Library/Application Support/org.pqrs/Karabiner-Elements/bin/karabiner_cli' --lint-complex-modifications $outfile
done
