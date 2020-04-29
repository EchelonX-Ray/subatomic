#!/bin/bash

# -----------------------------------------------------------------------------
# START: Configure The Build Parameters

TEST_FONT_FILE="/usr/share/fonts/dejavu/DejaVuSansMono.ttf"
TEST_FONT_FAMILY="DejaVu Serif"

LINK_STATICLLY="NO"

CFLAGS="-Wall -std=c99 -pipe -O2 -flto -march=native -g $(pkg-config --cflags x11)"
#CFLAGS="$CFLAGS -DDEVEL_STRIP_MCURSOR"
#CFLAGS="$CFLAGS -DDEVEL_STRIP_FONTLIST"

LDFLAGS=""

PKGCONFIG_LINK="--libs --cflags"

TUPLE="x86_64-pc-linux-gnu"

#   END: Configure The Build Parameters
# -----------------------------------------------------------------------------

# Get the location of this script in filesystem
get_script_dir() {
     SOURCE="${BASH_SOURCE[0]}"
     # While $SOURCE is a symlink, resolve it
     while [ -h "$SOURCE" ]; do
          DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
          SOURCE="$( readlink "$SOURCE" )"
          # If $SOURCE was a relative symlink (so no "/" as prefix, need to resolve it relative to the symlink base directory
          [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
     done
     DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
     echo "$DIR"
}
cd "$(get_script_dir)"

echo "Create build directory structure"
mkdir -p "/dev/shm/devel/subatomic/dev_build"
chmod 700 "/dev/shm/devel"
ln -s "/dev/shm/devel/subatomic/dev_build/" "./build"
mkdir -p "./build/core"
mkdir -p "./build/lib/toolbox"
mkdir -p "./build/lib/wincomp"
mkdir -p "./build/lib/wincomp/input"
mkdir -p "./build/lib/wincomp/elements"

FLDFLAGS=""
if [ "$LINK_STATICLLY" == "YES" ]; then
  CFLAGS="-static $CFLAGS"
  LDFLAGS="-static $LDFLAGS"
  FLDFLAGS="-lm -ldl"
  PKGCONFIG_LINK="--static $PKGCONFIG_LINK"
fi

# If the variable TUPLE is unset or empty, default to not using a prefix
CC=""
LD=""
if [ -z "$TUPLE" ]; then
  CC="gcc"
  LD="ld"
else
  CC="${TUPLE}-gcc"
  LD="${TUPLE}-ld"
fi

# Complile the core program
echo "Compile: subatomic.c"
$CC -c "./../src/core/subatomic.c" -o "./build/core/subatomic.obj" $CFLAGS -DTEST_FONT_FILE="\"$TEST_FONT_FILE\"" -DTEST_FONT_FAMILY="\"$TEST_FONT_FAMILY\"" &
echo "Compile: core/events.c"
$CC -c "./../src/core/events.c" -o "./build/core/events.obj" $CFLAGS &

# Compile the window input processing files
echo "Compile: input/keyboard.c"
$CC -c "./../src/lib/wincomp/input/keyboard.c" -o "./build/lib/wincomp/input/keyboard.obj" $CFLAGS &
echo "Compile: input/mouse.c"
$CC -c "./../src/lib/wincomp/input/mouse.c" -o "./build/lib/wincomp/input/mouse.obj" $CFLAGS &

# Compile the window elements
echo "Compile: elements/button.c"
$CC -c "./../src/lib/wincomp/elements/button.c" -o "./build/lib/wincomp/elements/button.obj" $CFLAGS &
echo "Compile: elements/checkbox.c"
$CC -c "./../src/lib/wincomp/elements/checkbox.c" -o "./build/lib/wincomp/elements/checkbox.obj" $CFLAGS &
echo "Compile: elements/container.c"
$CC -c "./../src/lib/wincomp/elements/container.c" -o "./build/lib/wincomp/elements/container.obj" $CFLAGS &
echo "Compile: elements/file_text.c"
$CC -c "./../src/lib/wincomp/elements/file_text.c" -o "./build/lib/wincomp/elements/file_text.obj" $CFLAGS &
echo "Compile: elements/ml_textbox.c"
$CC -c "./../src/lib/wincomp/elements/ml_textbox.c" -o "./build/lib/wincomp/elements/ml_textbox.obj" $CFLAGS &
echo "Compile: elements/radiobutton.c"
$CC -c "./../src/lib/wincomp/elements/radiobutton.c" -o "./build/lib/wincomp/elements/radiobutton.obj" $CFLAGS &
echo "Compile: elements/tab.c"
$CC -c "./../src/lib/wincomp/elements/tab.c" -o "./build/lib/wincomp/elements/tab.obj" $CFLAGS &
echo "Compile: elements/textbox.c"
$CC -c "./../src/lib/wincomp/elements/textbox.c" -o "./build/lib/wincomp/elements/textbox.obj" $CFLAGS &

# Compile the toolbox
echo "Compile: cstr_manip.c"
$CC -c "./../src/lib/toolbox/cstr_manip.c" -o "./build/lib/toolbox/cstr_manip.obj" $CFLAGS &

# Compile the base library components
echo "Compile: wincomp.c"
$CC -c "./../src/lib/wincomp/wincomp.c" -o "./build/lib/wincomp/wincomp.obj" $CFLAGS &
echo "Compile: wincomp/events.c"
$CC -c "./../src/lib/wincomp/events.c" -o "./build/lib/wincomp/events.obj" $CFLAGS &
echo "Compile: wincomp/drawing.c"
$CC -c "./../src/lib/wincomp/drawing.c" -o "./build/lib/wincomp/drawing.obj" $CFLAGS &
echo "Compile: wincomp/text.c"
$CC -c "./../src/lib/wincomp/text.c" -o "./build/lib/wincomp/text.obj" $CFLAGS &
echo "Compile: wincomp/elements.c"
$CC -c "./../src/lib/wincomp/elements.c" -o "./build/lib/wincomp/elements.obj" $CFLAGS &
echo "Compile: wincomp/font.c"
$CC -c "./../src/lib/wincomp/font.c" -o "./build/lib/wincomp/font.obj" $(pkg-config --cflags freetype2) $(pkg-config --cflags fontconfig) $CFLAGS &

# Wait for compilation to finish
echo "Waiting for compilation to finish."
wait
echo "Compilation: Complete"

# Link together the element object files
echo "Link: Element Objects"
$LD $LDFLAGS -r -o "./build/subatomic_p1.obj" \
  "./build/lib/wincomp/elements/button.obj" \
  "./build/lib/wincomp/elements/checkbox.obj" \
  "./build/lib/wincomp/elements/container.obj" \
  "./build/lib/wincomp/elements/file_text.obj" \
  "./build/lib/wincomp/elements/ml_textbox.obj" \
  "./build/lib/wincomp/elements/radiobutton.obj" \
  "./build/lib/wincomp/elements/tab.obj" \
  "./build/lib/wincomp/elements/textbox.obj" &

echo "Link: All Compiled Objects"
$LD $LDFLAGS -r -o "./build/subatomic_p2.obj" \
  "./build/core/subatomic.obj" \
  "./build/core/events.obj" \
  "./build/lib/toolbox/cstr_manip.obj" &

$LD $LDFLAGS -r -o "./build/subatomic_p3.obj" \
  "./build/lib/wincomp/wincomp.obj" \
  "./build/lib/wincomp/elements.obj" \
  "./build/lib/wincomp/events.obj" \
  "./build/lib/wincomp/drawing.obj" \
  "./build/lib/wincomp/text.obj" \
  "./build/lib/wincomp/font.obj" \
  "./build/lib/wincomp/input/keyboard.obj" \
  "./build/lib/wincomp/input/mouse.obj" &

wait
echo "Link: Against X11 + FreeType + FontConfig + pThread and Produce Finished Executable ./subatomic.out"
$CC $CFLAGS $LDFLAGS -o "./subatomic.out" \
  "./build/subatomic_p1.obj" \
  "./build/subatomic_p2.obj" \
  "./build/subatomic_p3.obj" \
  -lpthread $FLDFLAGS \
  $(pkg-config $PKGCONFIG_LINK freetype2) \
  $(pkg-config $PKGCONFIG_LINK fontconfig) \
  $(pkg-config $PKGCONFIG_LINK x11)

echo "Deleting Build Directory"
rm -rf "/dev/shm/devel"
rm -f "./build"

echo "Dev Build Finished"

exit 0
