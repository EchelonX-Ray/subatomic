#!/bin/bash

TEST_FONT_DIR="/usr/share/fonts/dejavu/DejaVuSansMono.ttf"
CFLAGS="-Wall -std=c99 -pipe -O2 -flto -march=native"

echo "Create build directory structure"
mkdir -p "/dev/shm/devel/subatomic/dev_build"
chmod 700 "/dev/shm/devel"
ln -s "/dev/shm/devel/subatomic/dev_build/" "./build"
mkdir -p "./build/core"
mkdir -p "./build/lib/toolbox"
mkdir -p "./build/lib/wincomp"
mkdir -p "./build/lib/wincomp/elements"

# Complile the core program
echo "Compile: subatomic.c"
gcc -c "./../src/core/subatomic.c" -o "./build/core/subatomic.obj" $CFLAGS -DTEST_FONT_DIR=\"$TEST_FONT_DIR\" &
echo "Compile: core/events.c"
gcc -c "./../src/core/events.c" -o "./build/core/events.obj" $CFLAGS &

# Compile the window elements
echo "Compile: elements/button.c"
gcc -c "./../src/lib/wincomp/elements/button.c" -o "./build/lib/wincomp/elements/button.obj" $CFLAGS &
echo "Compile: elements/checkbox.c"
gcc -c "./../src/lib/wincomp/elements/checkbox.c" -o "./build/lib/wincomp/elements/checkbox.obj" $CFLAGS &
echo "Compile: elements/container.c"
gcc -c "./../src/lib/wincomp/elements/container.c" -o "./build/lib/wincomp/elements/container.obj" $CFLAGS &
echo "Compile: elements/file_text.c"
gcc -c "./../src/lib/wincomp/elements/file_text.c" -o "./build/lib/wincomp/elements/file_text.obj" $CFLAGS &
echo "Compile: elements/ml_textbox.c"
gcc -c "./../src/lib/wincomp/elements/ml_textbox.c" -o "./build/lib/wincomp/elements/ml_textbox.obj" $CFLAGS &
echo "Compile: elements/radiobutton.c"
gcc -c "./../src/lib/wincomp/elements/radiobutton.c" -o "./build/lib/wincomp/elements/radiobutton.obj" $CFLAGS &
echo "Compile: elements/tab.c"
gcc -c "./../src/lib/wincomp/elements/tab.c" -o "./build/lib/wincomp/elements/tab.obj" $CFLAGS &
echo "Compile: elements/textbox.c"
gcc -c "./../src/lib/wincomp/elements/textbox.c" -o "./build/lib/wincomp/elements/textbox.obj" $CFLAGS &

# Compile the toolbox
echo "Compile: cstr_manip.c"
gcc -c "./../src/lib/toolbox/cstr_manip.c" -o "./build/lib/toolbox/cstr_manip.obj" $CFLAGS &

# Compile the base library components
echo "Compile: wincomp.c"
gcc -c "./../src/lib/wincomp/wincomp.c" -o "./build/lib/wincomp/wincomp.obj" $CFLAGS &
echo "Compile: wincomp/events.c"
gcc -c "./../src/lib/wincomp/events.c" -o "./build/lib/wincomp/events.obj" $CFLAGS &
echo "Compile: wincomp/drawing.c"
gcc -c "./../src/lib/wincomp/drawing.c" -o "./build/lib/wincomp/drawing.obj" $CFLAGS &
echo "Compile: wincomp/text.c"
gcc -c "./../src/lib/wincomp/text.c" -o "./build/lib/wincomp/text.obj" $CFLAGS &
echo "Compile: wincomp/elements.c"
gcc -c "./../src/lib/wincomp/elements.c" -o "./build/lib/wincomp/elements.obj" $CFLAGS &
echo "Compile: wincomp/font.c"
gcc -c "./../src/lib/wincomp/font.c" -o "./build/lib/wincomp/font.obj" $(pkg-config --cflags freetype2) $CFLAGS &

# Wait for compilation to finish
echo "Waiting for compilation to finish."
wait
echo "Compilation: Complete"

# Link together the element object files
echo "Link: All & Against X11 + FreeType + XML2 and Produce Finished Executable ./subatomic.out"
gcc \
  $(pkg-config --libs x11) \
  -lpthread \
  $(pkg-config --libs -cflags freetype2) \
  $CFLAGS -o "./subatomic.out" \
  "./build/lib/wincomp/wincomp.obj" \
  "./build/lib/wincomp/elements.obj" \
  "./build/lib/wincomp/events.obj" \
  "./build/lib/wincomp/drawing.obj" \
  "./build/lib/wincomp/text.obj" \
  "./build/lib/wincomp/font.obj" \
  "./build/core/subatomic.obj" \
  "./build/core/events.obj" \
  "./build/lib/toolbox/cstr_manip.obj" \
  "./build/lib/wincomp/elements/button.obj" \
  "./build/lib/wincomp/elements/checkbox.obj" \
  "./build/lib/wincomp/elements/container.obj" \
  "./build/lib/wincomp/elements/file_text.obj" \
  "./build/lib/wincomp/elements/ml_textbox.obj" \
  "./build/lib/wincomp/elements/radiobutton.obj" \
  "./build/lib/wincomp/elements/tab.obj" \
  "./build/lib/wincomp/elements/textbox.obj"

echo "Deleting Build Directory"
rm -rf "/dev/shm/devel"
rm -f "./build"

echo "Dev Build Finished"
exit 0
