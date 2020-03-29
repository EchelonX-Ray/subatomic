#!/bin/bash

CFLAGS="-pipe -O2 -flto -march=native"

echo "Create build directory structure"
mkdir -p "./build"
mkdir -p "./build/core"
mkdir -p "./build/lib/toolbox"
mkdir -p "./build/lib/wincomp"

# Complile the core program
echo "Compile: subatomic.c"
gcc -c "./../src/core/subatomic.c" -o "./build/core/subatomic.obj" $CFLAGS &
echo "Compile: core/events.c"
gcc -c "./../src/core/events.c" -o "./build/core/events.obj" $CFLAGS &

# Compile the base library components
echo "Compile: wincomp.c"
gcc -c "./../src/lib/wincomp/wincomp.c" -o "./build/lib/wincomp/wincomp.obj" $CFLAGS &
echo "Compile: wincomp/events.c"
gcc -c "./../src/lib/wincomp/events.c" -o "./build/lib/wincomp/events.obj" $CFLAGS &
echo "Compile: wincomp/drawing.c"
gcc -c "./../src/lib/wincomp/drawing.c" -o "./build/lib/wincomp/drawing.obj" $CFLAGS &

# Wait for compilation to finish
echo "Waiting for compilation to finish."
wait
echo "Compilation: Complete"

echo "Link: All Compiled Objects"
gcc $CFLAGS -r -o "./build/subatomic.obj" \
  "./build/core/subatomic.obj" \
  "./build/core/events.obj" \
  "./build/lib/wincomp/wincomp.obj" \
  "./build/lib/wincomp/events.obj" \
  "./build/lib/wincomp/drawing.obj" \


echo "Link: Against X11 and Produce Finished Executable ./subatomic.out"
gcc -lX11 -o "./subatomic.out" "./build/subatomic.obj" $CFLAGS

echo "Deleting Build Directory"
rm -rf "./build"

echo "Dev Build Finished"
exit 0
