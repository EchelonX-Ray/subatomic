#!/bin/bash

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
gcc -c "./../src/core/subatomic.c" -o "./build/core/subatomic.obj" $CFLAGS &
echo "Compile: core/events.c"
gcc -c "./../src/core/events.c" -o "./build/core/events.obj" $CFLAGS &
wait

# Compile the window elements
echo "Compile: elements/button.c"
gcc -c "./../src/lib/wincomp/elements/button.c" -o "./build/lib/wincomp/elements/button.obj" $CFLAGS &
echo "Compile: elements/checkbox.c"
gcc -c "./../src/lib/wincomp/elements/checkbox.c" -o "./build/lib/wincomp/elements/checkbox.obj" $CFLAGS &
echo "Compile: elements/container.c"
gcc -c "./../src/lib/wincomp/elements/container.c" -o "./build/lib/wincomp/elements/container.obj" $CFLAGS &
echo "Compile: elements/ml_textbox.c"
gcc -c "./../src/lib/wincomp/elements/ml_textbox.c" -o "./build/lib/wincomp/elements/ml_textbox.obj" $CFLAGS &
echo "Compile: elements/radiobutton.c"
gcc -c "./../src/lib/wincomp/elements/radiobutton.c" -o "./build/lib/wincomp/elements/radiobutton.obj" $CFLAGS &
echo "Compile: elements/tab.c"
gcc -c "./../src/lib/wincomp/elements/tab.c" -o "./build/lib/wincomp/elements/tab.obj" $CFLAGS &
echo "Compile: elements/textbox.c"
gcc -c "./../src/lib/wincomp/elements/textbox.c" -o "./build/lib/wincomp/elements/textbox.obj" $CFLAGS &
wait
# Link together the element object files
echo "Link: Element Objects"
gcc $CFLAGS -r -o "./build/lib/wincomp/elements/ALL_Elements.obj" \
  "./build/lib/wincomp/elements/button.obj" \
  "./build/lib/wincomp/elements/checkbox.obj" \
  "./build/lib/wincomp/elements/container.obj" \
  "./build/lib/wincomp/elements/ml_textbox.obj" \
  "./build/lib/wincomp/elements/radiobutton.obj" \
  "./build/lib/wincomp/elements/tab.obj" \
  "./build/lib/wincomp/elements/textbox.obj"

# Compile the toolbox
echo "Compile cstr_manip.c"
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
#echo "Compile: wincomp/element_parsing.c"
#gcc -c "./../src/lib/wincomp/element_parsing.c" -o "./build/lib/wincomp/element_parsing.obj" $(xml2-config --cflags) $CFLAGS &
echo "Compile: wincomp/font.c"
gcc -c "./../src/lib/wincomp/font.c" -o "./build/lib/wincomp/font.obj" $(freetype-config --cflags) $CFLAGS &

# Wait for compilation to finish
echo "Waiting for compilation to finish."
wait
echo "Compilation: Complete"

echo "Link: All Compiled Objects"
gcc $CFLAGS -r -o "./build/subatomic_p1.obj" \
  "./build/core/subatomic.obj" \
  "./build/core/events.obj" \
  "./build/lib/wincomp/wincomp.obj" \
  "./build/lib/wincomp/elements.obj" \
  "./build/lib/wincomp/elements/ALL_Elements.obj" \
  "./build/lib/wincomp/events.obj" \
  "./build/lib/wincomp/drawing.obj" \
  "./build/lib/wincomp/text.obj" \
  "./build/lib/wincomp/font.obj"
#  "./build/lib/wincomp/element_parsing.obj"

gcc $CFLAGS -r -o "./build/subatomic_p2.obj" \
  "./build/lib/toolbox/cstr_manip.obj"

echo "Link: Against X11 + FreeType + XML2 and Produce Finished Executable ./subatomic.out"
gcc \
  -lX11 \
  $(freetype-config --libs) $(freetype-config --cflags) \
  $(xml2-config --cflags) $(xml2-config --libs) \
  $CFLAGS -o "./subatomic.out" \
  "./build/subatomic_p1.obj" \
  "./build/subatomic_p2.obj"

echo "Deleting Build Directory"
rm -rf "/dev/shm/devel"
rm -f "./build"

echo "Dev Build Finished"
exit 0
