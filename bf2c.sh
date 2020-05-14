#!/bin/bash

set -e

outfile=a.out

if [ $# -eq 2 ]; then
  outfile=$2
fi

cmake-build-debug/interpreter test/bf_to_c.bf <"$1" >"$outfile.c"
gcc "$outfile.c" -o "$outfile"
rm -f "$outfile.c"
