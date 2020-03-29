#!/bin/bash

echo "Make build directory structure"
mkdir -p "./build"
mkdir -p "./build/core"
mkdir -p "./build/lib/toolbox"
mkdir -p "./build/lib/wincomp"

echo "Compile: subatomic.c"
gcc -c "./../src/core/subatomic.c" -o "./build/core/subatomic.obj" -O2 -flto -march=native &
echo "Compile: core/events.c"
gcc -c "./../src/core/events.c" -o "./build/core/events.obj" -O2 -flto -march=native &
echo "Compile: wincomp.c"
gcc -c "./../src/lib/wincomp/wincomp.c" -o "./build/lib/wincomp/wincomp.obj" -O2 -flto -march=native &
echo "Compile: wincomp/events.c"
gcc -c "./../src/lib/wincomp/events.c" -o "./build/lib/wincomp/events.obj -O2" -flto -march=native &

echo "Waiting for compilation to finish."
wait
echo "Compilation: Complete"

echo "Link: All Compiled Objects"
gcc -r -o "./build/subatomic.obj" \
  "./build/core/subatomic.obj" \
  "./build/core/events.obj" \
  "./build/lib/wincomp/wincomp.obj" \
  "./build/lib/wincomp/events.obj"

echo "Link: Against X11 and Produce Finished Executable ./subatomic.out"
gcc -lX11 -o "./subatomic.out" "./build/subatomic.obj" -O2 -flto -march=native

rm -rf "./build"

exit 0
