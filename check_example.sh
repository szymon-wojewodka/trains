#!/usr/bin/env bash
make -B example >/dev/null || { echo failed to compile; exit 1; }
diff --color -u <(./example) trains_example.out || { echo The output differs; exit 2; } 

