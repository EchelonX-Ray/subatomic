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
echo "Compile: wincomp/text.c"
gcc -c "./../src/lib/wincomp/text.c" -o "./build/lib/wincomp/text.obj" $CFLAGS &
echo "Compile: wincomp/font.c"
gcc -c "./../src/lib/wincomp/font.c" -o "./build/lib/wincomp/font.obj" $(freetype-config --cflags) $CFLAGS &

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
  "./build/lib/wincomp/text.obj" \
  "./build/lib/wincomp/font.obj"


echo "Link: Against X11 + FreeType and Produce Finished Executable ./subatomic.out"
gcc -lX11 $(freetype-config --libs) $(freetype-config --cflags) $CFLAGS -o "./subatomic.out" \
  "./build/subatomic.obj"


echo "Deleting Build Directory"
rm -rf "./build"

echo "Dev Build Finished"
exit 0
