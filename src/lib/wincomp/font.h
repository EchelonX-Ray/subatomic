#ifndef _insertion_lib_wincomp_font_h
#define _insertion_lib_wincomp_font_h

#include "./wincomp.h"
#include "./text.h"

// I am only going to deal with 1 font style for now.  I'll change this later.
#define MTK_FONT_STYLE_COUNT 1
#define MTK_FONT_STYLE_NORMAL 0
#define MTK_FONT_STYLE_BOLD 1
#define MTK_FONT_STYLE_ITALIC 2
// Set to the first printable ASCII charactor. (0x20 = [Space])
#define MTK_FONT_ASCII_CHAR_MIN 0x20
// Set to the last printable ASCII charactor. (0x7E = "~")
// There may be more ASCII charactors, but I don't want to deal with them for now
#define MTK_FONT_ASCII_CHAR_MAX 0x7E

void load_font_family(char *family, struct MTK_WinFontPack *font_pack);
void setup_font(unsigned int size, struct MTK_WinFontPack *font_pack);
void free_font(struct MTK_WinFontPack *font_pack);

#endif
